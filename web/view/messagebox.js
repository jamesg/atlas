var _ = require('underscore');
var Backbone = require('backbone');
Backbone.$ = require('jbone');
var domjs = require('domjs/lib/html5')(document);

var CollectionView = require('./collection').CollectionView;
var StaticView = require('./static').StaticView;

var defaultTimeout = 5000;

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

var MessageView = StaticView.extend(
    {
        model: Message,
        className: function() {
            return 'messagebox messagebox-' + this.model.get('severity');
        },
        fadeout: function() {
            this.$el.attr('style', 'opacity: 0;')
        },
        template: function() {
            span(this.model.get('message')),
            div(
                { class: 'messagebox-close' },
                button(
                    {
                        class: 'display-link',
                        onclick: this.model.destroy.bind(this.model)
                    },
                    'Close'
                    )
                );
        }
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

exports.MessageBox = Backbone.View.extend(
    {
        initialize: function(options) {
            if(_.has(options, 'model'))
                this.model = options.model;
            if(!_.has(this, 'model'))
                this.model = new MessageCollection;
            this.collectionView = new MessageCollectionView({ model: this.model });
            this.collectionView.render();
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
            this.el.appendChild(this.collectionView.el);
            return this;
        }
    }
    );

