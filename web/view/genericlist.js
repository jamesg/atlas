var domjs = require('domjs')(document);
var _ = require('underscore');
var Backbone = require('backbone');
Backbone.$ = require('jbone');

var CollectionView = require('./collection').CollectionView;

var StaticView = require('./static').StaticView;
var ui = require('../ui');

var LiView = StaticView.extend(
    {
        tagName: 'li',
        className: 'editable-li',
        template: function() {
            a(
                {
                    onclick: this.trigger.bind(this, 'click', this.model)
                },
                this.model.get('name')
                );
        }
    }
    );

var UlView = CollectionView.extend(
    {
        initialize: function(options) {
            CollectionView.prototype.initialize.apply(this, arguments);
        },
        tagName: 'ul',
        initializeView: function(view) {
            this.listenTo(view, 'click', this.trigger.bind(this, 'click'));
        }
    }
   );

exports.GenericListView = Backbone.View.extend(
    {
        tagName: 'div',
        className: 'pure-menu pure-menu-vertical pure-menu-open',
        initialize: function(options) {
            if(_.has(options, 'view'))
                this.view = options.view;
            console.log('listview view', this.view);
            if(!_.isFunction(this['view']))
            {
                this.view = LiView;
                console.log('assign view');
            }
            this.ulView = new UlView({ model: this.model, view: this.view });
            this.listenTo(this.ulView, 'click', this.trigger.bind(this, 'click'));
            this.render();
        },
        render: function() {
            this.$el.empty();
            this.el.appendChild(this.ulView.el);
            return this;
        },
        collectionView: function() {
            return this.ulView;
        }
    }
    );

