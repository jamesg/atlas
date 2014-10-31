var _ = require('underscore');
var ds = require('../service/ds');
var MessageBox = require('../view/messagebox').MessageBox;
var PageView = require('../view/page').PageView;
var StaticView = require('../view/static').StaticView;
var User = require('../model/user').User;
var ui = require('../ui');

var UserForm = StaticView.extend(
    {
        initialize: function() {
            this._messageBox = new MessageBox;
            this.render();
        },
        setModel: function(model) {
            this.model = model;
            this.render();
        },
        template: function() {
            var username = input({ type: 'text', value: this.model.get('username') });
            var enabled = input({ type: 'checkbox', checked: this.model.get('user_enabled') });
            var super_ = input({ type: 'checkbox', checked: this.model.get('user_super') });
            var buttonBox = div(
                    { class: 'buttonbox' },
                    button(
                        {
                            type: 'button',
                            class: 'pure-button',
                            onclick: (function() {
                                this.setModel(new User);
                            }).bind(this)
                        },
                        ui.icon('cross'), 'Clear'
                        )
                    );
            if(!this.model.isNew())
            {
                buttonBox(
                    ui.confirmButton(
                        _.partial(span, ui.icon('delete'), 'Delete'),
                        (function() {
                            this.model.destroy();
                            this.setModel(new User);
                            this.trigger('saved');
                        }).bind(this)
                        )
                    );
            }
            buttonBox(
                button(
                    {
                        type: 'submit',
                        class: 'pure-button pure-button-primary'
                    },
                    ui.icon('data-transfer-download'), (this.model.isNew()?'Create':'Update')
                    )
                );
            return form(
                {
                    class: 'pure-form pure-form-aligned',
                    onsubmit: (function() {
                        this.model.set('username', username().value);
                        this.model.set('user_enabled', enabled().checked)
                        this.model.set('user_super', super_().checked);
                        if(this.model.isValid()) {
                            this.model.save().then(
                                (function() {
                                    this._messageBox.displaySuccess('User details saved');
                                    this.trigger('saved');
                                }).bind(this)
                                );
                        } else {
                            this._messageBox.displayError(
                                ds.errorString(this.model.validationError)
                                );
                        }
                        return false;
                    }).bind(this)
                },
                this._messageBox.el,
                fieldset(
                    ui.inlineInput('Username', username),
                    ui.inlineCheckInput(enabled, 'Account enabled'),
                    ui.inlineCheckInput(super_, 'Superuser')
                    ),
                buttonBox
                );
        }
    }
    );

exports.UserAdmin = PageView.extend(
    {
        pageTitle: function() {
            return this.model.get('username');
        },
        initialize: function(options) {
            if(_.has(this.options, 'model'))
                this.model = options.model;
            if(!_.has(this, 'model'))
                this.model = new User;
            this.form = new UserForm({ model: this.model });

            PageView.prototype.initialize.apply(this, arguments);

            this.listenTo(
                this.form,
                'saved',
                this.application.popPage.bind(this.application)
                );
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('User'),
                    this.form.el
                   )
                );
        }
    }
    );

