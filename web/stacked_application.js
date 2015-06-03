/*
 * Functions for accessing the browser's local storage API.  Some old browsers
 * do not support local storage; fall back to a global array if this is the
 * case.
 */
var storage = {};

if(
    (function() {
        console.log('testing localStorage');
        try {
            localStorage.setItem('mod', 'mod');
            localStorage.removeItem('mod');
            console.log('found localStorage');
            return true;
        } catch(exception) {
            console.log('did not find localStorage');
            return false;
        }
    }()
    ))
{
    storage = {
        get: function(key) {
            return window.localStorage.getItem(key);
        },
        has: function(key) {
            return _.has(gStorage, key);
        },
        set: function(key, value) {
            window.localStorage.setItem(key, value);
        },
        remove: function(key) {
            window.localStorage.setItem(key, null);
        }
    };
} else {
    var gStorage = {};
    storage = {
        get: function(key) {
            return gStorage[key];
        },
        has: function(key) {
            return (window.localStorage.getItem(key) != null);
        },
        set: function(key, value) {
            gStorage[key] = value;
        },
        remove: function(key) {
            delete gStorage['key'];
        }
    };
}

/*
 * Set the 'Authorization' header on an outgoing XMLHttpRequest.
 */
var setHeader = function(xhr) {
    xhr.setRequestHeader('Authorization', storage.get('token'));
};

{
    var oldSync = Backbone.sync;
    Backbone.sync = function(method, model, options) {
        options.beforeSend = setHeader;
        oldSync(method, model, options);
    };
}

/*
 * Push a sign in page to the application whenever a jQuery request receives a
 * 403 error.
 */
$(document).ajaxError(
        function (e, xhr, options) {
            gApplication.pushPage(SignInPage);
        }
        );

/*
 * Make a JSONRPC request.
 *
 * 'options' is a map of:
 *
 * success: Function called with the JSONRPC result if the request is successful.
 * error: Function called with the JSONRPC error message if the request fails.
 * url: JSONRPC endpoint.
 * method: JSONRPC method to call.
 * params: Parameters to the method.
 */
var jsonRpc = function(options) {
    var url = _.has(options, 'url')?options.url:'api_call';
    var req = _.has(options, 'xhr')?options.xhr:new XMLHttpRequest;
    var reqListener = function() {
        switch(this.status) {
            case 200:
                var jsonIn = JSON.parse(this.responseText);
                if(_.has(jsonIn, 'result'))
                    options.success(jsonIn.result);
                else if(_.has(jsonIn, 'error'))
                    options.error(jsonIn.error);
                else
                    options.error();
                break;
            case 403:
                gApplication.pushPage(SignInPage);
                break;
        }
    }

    var requestContent = _.pick(options, 'method', 'params');
    console.log(requestContent);
    req.open('post', url, true);
    setHeader(req);
    req.onload = reqListener;
    req.send(JSON.stringify(requestContent));
};

var StaticView = Backbone.View.extend(
    {
        initialize: function(options) {
            Backbone.View.prototype.initialize.apply(this, arguments);
            if(_.has(this, 'model') && !_.isUndefined(this.model))
                this.listenTo(this.model, 'change', this.render.bind(this));
            if(!_.isUndefined(options) && _(options).has('template'))
                this.template = options.template;
        },
        template: '',
        templateParams: function() {
            return (_(this).has('model') && !_.isUndefined(this.model))?
                this.model.toJSON():{};
        },
        render: function() {
            if(_.isFunction(this['template']))
                this.$el.html(this.template(this.templateParams()));
            else
                this.$el.html(_.template(this.template)(this.templateParams()));
        }
    }
    );

var ModalButton = Backbone.Model.extend(
    {
        defaults: {
            action: function() {},
            label: 'Button',
            name: 'close',
            icon: 'check'
        }
    }
    );

var ModalButtonView = StaticView.extend(
    {
        events: {
            'click button': 'triggerClick'
        },
        tagName: 'span',
        template: '\
        <button type="button" name="<%-name%>">\
            <span class="oi" data-glyph="<%-icon%>" aria-hidden="true"> </span>\
            <%-label%>\
        </button>\
        ',
        triggerClick: function() {
            this.model.trigger('click');
        }
    }
    );

var ModalButtonCollection = Backbone.Collection.extend(
    {
        model: ModalButton,
        comparator: function(model) {
            var index = ['no', 'cancel', 'close', 'destroy', 'save', 'yes', 'prev', 'next']
                .indexOf(model.get('name'));
            return (index > -1) ? index : model.get('name');
        }
    }
    );

var StandardButton = {
    close: function(action) {
        return { name: 'close', icon: 'x', label: 'Close', action: action };
    },
    cancel: function(action) {
        return { name: 'cancel', icon: 'x', label: 'Cancel', action: action };
    },
    destroy: function(action) {
        return { name: 'destroy', icon: 'trash', label: 'Delete', action: action };
    },
    no: function(action) {
        return { name: 'no', icon: 'x', label: 'No', action: action };
    },
    yes: function(action) {
        return { name: 'yes', icon: 'check', label: 'Yes', action: action };
    },
    create: function(action) {
        return { name: 'create', icon: 'file', label: 'Create', action: action };
    },
    ok: function(action) {
        return { name: 'ok', icon: 'check', label: 'Ok', action: action };
    },
    save: function(action) {
        return { name: 'save', icon: 'data-transfer-download', label: 'Save', action: action };
    },
    prev: function(action) {
        return { name: 'prev', icon: 'chevron-left', label: 'Prev', action: action };
    },
    next: function(action) {
        return { name: 'next', icon: 'chevron-right', label: 'Next', action: action };
    }
};

var Modal = Backbone.View.extend(
    {
        /*
         * options - map of:
         *     buttons - buttons to display
         *     view - view constructor
         *     model - passed to the view
         */
        initialize: function(options) {
            Backbone.View.prototype.initialize.apply(this, arguments);

            this.$el.append(_.template(this.template)(this.templateParams()));

            var buttons = coalesce(
                options['buttons'],
                this['buttons'],
                [ StandardButton.close() ]
                );
            this._buttons = new ModalButtonCollection(buttons);
            this._buttons.each(
                function(button) {
                    this.listenTo(button, 'click', this._end.bind(this, button));
                },
                this
                );

            (new CollectionView({
                model: this._buttons,
                view: ModalButtonView,
                el: this.$('[name=buttons]')
            })).render();

            var cons = coalesce(options['view'], this['view']);
            this._view = new cons({ el: this.contentEl(), model: this['model']});
            this._view.render();
            this.listenTo(this._view, 'finished', this.finish.bind(this));
        },
        className: 'modal',
        template: '\
<div class="modal-dialog">\
    <form class="modal-content" name="modal-content">\
    </form>\
    <div class="modal-button-box" name="buttons">\
    </div>\
</div>\
            ',
        templateParams: function() {
            return { buttons: this._buttons };
        },
        contentEl: function() {
            return this.$('*[name=modal-content]')[0];
        },
        events: {
            'click button[name=cancel]': 'remove',
            'click button[name=close]': 'remove',
            'click button[name=destroy]': function() { return this._end('destroy'); },
            'click button[name=no]': function() { return this._end('no') },
            'click button[name=create]': function() { return this._end('create'); },
            'click button[name=save]': function() { return this._end('save'); },
            'click button[name=yes]': function() { return this._end('yes'); },
            'submit form': function() { return this._end('save'); }
        },
        finish: function() {
            this.remove();
            this.trigger('finished');
        },
        _end: function(button) {
            console.log('_end');
            var action = button.get('action');
            if(_.isFunction(action))
                action.apply(this);
            if(['cancel', 'close'].indexOf(button.get('name')) > -1)
                this.remove();
            this._view.trigger(button.get('name'));
            this._view.trigger('finished');
            this.trigger(button.get('name'));
            return false;
        }
    }
    );

var ConfirmModal = Modal.extend(
    {
        initialize: function(options) {
            Modal.prototype.initialize.apply(this, arguments);
            this._callback =
                coalesce(options['callback'], this['callback'], function() {});
            this._view.message = options['message'];
            this._view.render();
        },
        events: {
            'click button[name=no]': 'no',
            'click button[name=yes]': 'yes'
        },
        yes: function() {
            this._callback();
            this.remove();
        },
        no: function() {
            this.remove();
        },
        buttons: [ StandardButton.yes(), StandardButton.no() ],
        view: StaticView.extend({
            template: '<p><%-message%>',
            templateParams: function() { return { message: this.message }; },
            message: 'Are you sure?'
        })
    }
    );

/*
 * Return the first defined, non-null argument.  Call with any number of
 * arguments.
 *
 * If no arguments are defined and non-null, return null.
 */
var coalesce = function() {
    for(a in arguments)
        if(!_.isNull(arguments[a]) && !_.isUndefined(arguments[a]))
            return arguments[a];
    return null;
};

var CollectionView = Backbone.View.extend(
    {
        initialize: function(options) {
            this.view = coalesce(
                options['view'],
                this['view'],
                function() { console.log('warning: view not defined'); }
                );
            this.filter = coalesce(
                options['filter'],
                this['filter'],
                function(model) { return true; }
                );
            this.emptyView = coalesce(options['emptyView'], this['emptyView']);
            this._offset = coalesce(options['offset'], 0);
            this._limit = coalesce(options['limit'], -1);
            this._rendered = false;
            this._views = [];
            this.model.each(this.add, this);
            this.listenTo(this.model, 'add', this.add);
            this.listenTo(this.model, 'remove', this.remove);
            this.listenTo(this.model, 'reset', this.reset);
        },
        add: function(model, options) {
            var view = this.constructView(model);

            var pos = 0;
            // Increment pos until the next view in this._views has a greater
            // model index than the new view.
            while(
                pos < this._views.length &&
                this.model.indexOf(this._views[pos].model) < this.model.indexOf(model)
                )
                pos++;

            this._views.splice(pos, 0, view);

            this.render();
            this.trigger('add');
        },
        remove: function(model) {
            var viewToRemove = this.get(model);
            this._views = _(this._views).without(viewToRemove);
            this.render();
            this.trigger('remove');
        },
        get: function(model) {
            return _(this._views).select(
                function(cv) { return cv.model === model; }
                )[0];
        },
        reset: function() {
            this._views = [];
            this._rendered = false;
            this.model.each(
                function(model) {
                    var view = this.constructView(model);
                    this._views.push(view);
                    },
                this
                );
            this.render();
            this.trigger('reset');
        },
        constructView: function(model) {
            var view = new this.view({ model: model });
            this.initializeView(view);
            view.render();
            return view;
        },
        initializeView: function(view) {
        },
        emptyView: StaticView.extend({}),
        each: function(f, context) {
            if(_.isUndefined(context))
                _(this._views).each(f);
            else
                _(this._views).each(f, context);
        },
        render: function() {
            // Replace the content of the element with a newly constructed view
            // for each model in the collection.
            //
            // TODO reuse old models, both for performance and to enable
            // editable views.
            this._rendered = true;
            this.$el.empty();

            var filtered = _.filter(
                    this._views,
                    function(view) {
                        return this.filter(view.model);
                    },
                    this
                    );

            var views;
            if(this._limit >= 0)
                views = filtered.slice(this._offset, this._offset + this._limit);
            else
                views = filtered.slice(this._offset);

            if(views.length == 0) {
                if(!(_.has(this, '_emptyView'))) {
                    this._emptyView = new this.emptyView;
                    this._emptyView.render();
                }
                this.$el.append(this._emptyView.el);
            } else {
                if(_.has(this, '_emptyView')) {
                    this._emptyView.$el.remove();
                    delete this._emptyView;
                }
                _(views).each(
                    function(dv) {
                        this.$el.append(dv.el);
                        // Rendering the CollectionView in this way removes
                        // every element from the DOM and reinserts it
                        // somewhere else.  Event bindings are lost.
                        dv.delegateEvents();
                    },
                    this
                    );
            }
        }
    }
    );

/*
 * Single message to be visualised in a MessageBox.
 */
var Message = Backbone.Model.extend(
    {
        defaults: { severity: 'information', message: '', closeButton: true },
        timeout: function(delay) {
            setTimeout(
                (function() {
                    this.trigger('fadeout');
                    setTimeout(this.destroy.bind(this), 1000);
                }).bind(this),
            delay
            );
        }
    }
    );

var MessageCollection = Backbone.Collection.extend(
    {
        model: Message
    }
    );

/*
 * A single message displayed in a box with appropriate styling.
 */
var MessageView = StaticView.extend(
    {
        initialize: function() {
            StaticView.prototype.initialize.apply(this, arguments);
            this.render();
        },
        model: Message,
        className: function() {
            return 'messagebox messagebox-' + this.model.get('severity');
        },
        fadeout: function() {
            this.$el.attr('style', 'opacity: 0;')
        },
        events: {
            'click button[name=close]': function() { this.model.destroy(); }
        },
        template: '<span><%-message%></span><button name="close">Close</button>'
    }
    );

var MessageCollectionView = CollectionView.extend(
    {
        view: MessageView,
        initializeView: function(view) {
            this.listenTo(view.model, 'fadeout', view.fadeout.bind(view));
        }
    }
    );

/*
 * Default message box dismissal timeout in milliseconds.
 */
var defaultTimeout = 5000;

/*
 * View onto a list of messages which inform the user of recent events.
 * Messages may be dismissed by clicking the (optional) close button, or will
 * be dismissed automatically after a timeout.
 */
var MessageBox = StaticView.extend(
    {
        initialize: function(options) {
            StaticView.prototype.initialize.apply(this, arguments);
            if(!_.has(this, 'model'))
                this.model = new MessageCollection;
            this._collectionView = new MessageCollectionView({ model: this.model });
            this._collectionView.render();
            this.render();
        },
        displayError: function(str) {
            var message = new Message({ severity: 'error', message: str });
            message.timeout(defaultTimeout);
            this.model.add(message);
        },
        displayInformation: function(str) {
            var message = new Message({ severity: 'information', message: str });
            message.timeout(defaultTimeout);
            this.model.add(message);
        },
        displaySuccess: function(str) {
            var message = new Message({ severity: 'success', message: str });
            message.timeout(defaultTimeout);
            this.model.add(message);
        },
        displayWarning: function(str) {
            var message = new Message({ severity: 'warning', message: str });
            message.timeout(defaultTimeout);
            this.model.add(message);
        },
        render: function() {
            this.$el.empty();
            this.$el.append(this._collectionView.$el);
        }
    }
    );

var Breadcrumb = Backbone.Model.extend(
    {
        defaults: {
            page_title: '',
            view: null
        }
    }
    );

var BreadcrumbCollection = Backbone.Collection.extend(
    {
        model: Breadcrumb
    }
    );

var BreadcrumbView = StaticView.extend(
    {
        tagName: 'li',
        revisit: function revisit() {
            gApplication.revisit(this.model);
        },
        events: {
            'click a': 'revisit'
        },
        template:
            '<span aria-hidden="aria-hidden" data-glyph="chevron-right" class="oi"> </span>' +
            '<a><%-page_title%></a>  '
    }
    );

var BreadcrumbsView = CollectionView.extend(
    {
        view: BreadcrumbView,
        initialize: function(options) {
            this.application = options.application;
            CollectionView.prototype.initialize.apply(this, arguments);
            this.render();
        },
        initializeView: function(view) {
            view.application = this.application;
            view.render();
        },
        tagName: 'ul'
    }
    );

var StackedApplication = function(homeView) {
    this._homeView = homeView;

    this.breadcrumbs = new BreadcrumbCollection;
    this.navigation = new CollectionView(
            {
                el: $('#navigation'),
                model: this.breadcrumbs,
                view: BreadcrumbView
            }
            );

    //this.goHome();
};

//_.extend(StackedApplication, Backbone.Events);

StackedApplication.prototype._setElement = function(el) {
    document.getElementById('template-content').innerHTML = '';
    document.getElementById('template-content').appendChild(el);
};

StackedApplication.prototype._createPage = function(constructor) {
    return (_.isFunction(constructor))?
        (new constructor({ application: this })):
        constructor;
};

StackedApplication.prototype._createBreadcrumb = function(view) {
    return new Breadcrumb(
        {
            page_title: _.isFunction(view.pageTitle)?
                    view.pageTitle():view.pageTitle,
            view: view
        }
        );
};

StackedApplication.prototype._setPage = function(view) {
    view.render();
    this._setElement(view.el);
};

StackedApplication.prototype.goHome = function() {
    this.breadcrumbs.reset();
    var home = this._createPage(this._homeView);
    this.breadcrumbs.add(this._createBreadcrumb(home));
    this._setPage(home);
};

StackedApplication.prototype.gotoPage = function(constructor) {
    this.breadcrumbs.reset();
    var home = this._createPage(HomePage);
    this.breadcrumbs.add(this._createBreadcrumb(home));
    this.pushPage(constructor);
};

StackedApplication.prototype.pushPage = function(constructor) {
    var view = this._createPage(constructor);
    this.breadcrumbs.add(this._createBreadcrumb(view));
    this._setPage(view);
};

StackedApplication.prototype.popPage = function() {
    this.breadcrumbs.pop();
    if(this.breadcrumbs.length == 0)
        this.goHome();
    else
        this._setPage(this.breadcrumbs.at(this.breadcrumbs.length-1).get('view'));
    this.currentPage().reset();
};

StackedApplication.prototype.currentPage = function() {
    if(this.breadcrumbs.length < 1)
        throw 'there is not current page';
    return this.breadcrumbs.at(this.breadcrumbs.length-1).get('view');
};

StackedApplication.prototype.revisit = function(breadcrumb) {
    var i = 0;
    while(i < this.breadcrumbs.length) {
        if(this.breadcrumbs.at(i) == breadcrumb) {
            this.breadcrumbs.remove(
                    this.breadcrumbs.slice(i+1, this.breadcrumbs.length)
                    );
            this._setPage(this.breadcrumbs.at(i).get('view'));
            this.currentPage().reset();
            return;
        }
        ++i;
    }
};

/*
 * Display a modal dialog.
 */
StackedApplication.prototype.modal = function(modal) {
    $('#modal-container').append(modal.el);
};

/*!
 * \brief Handle an authentication error by displaying a sign in page over the
 * current page.  Will only display a sign in page if none is currently
 * displayed.
 */
StackedApplication.prototype.authenticationError = function() {
    if(!(this.currentPage() instanceof AuthenticationRequiredPage)) {
        this.pushPage(AuthenticationRequiredPage);
    }
};

var PageView = StaticView.extend(
    {
        pageTitle: 'Untitled Page',
        reset: function() { }
    }
    );

// REST responses consist of the actual response as the 'data' key inside a
// JSON object.  This allows the protocol to be extended more easily at a later
// date if required.  In the case of array responses, returning a raw array is
// also a potential security risk.

var RestModel = Backbone.Model.extend(
        {
            parse: function(response) {
                if(_.has(response, 'data'))
                    return response.data;
                else
                    return response;
            }
        }
        );

var RestCollection = Backbone.Collection.extend(
        {
            parse: function(response) {
                return response.data;
            }
        }
        );

