var domjs = require('domjs')(document);
var _ = require('underscore');
var Backbone = require('backbone');
Backbone.$ = require('jbone');

exports.CollectionView = Backbone.View.extend(
    {
        initialize: function(options) {
            this.render();
            _(this).bindAll('add', 'remove');
            //if(!_.has(this, 'view') && !_.has(options, 'view'))
                //throw 'no child view constructor provided';
            if(_.has(options, 'view')) this.view = options.view;
            this._views = [];
            this.model.each(this.add);
            this.listenTo(this.model, 'add', this.add);
            this.listenTo(this.model, 'remove', this.remove);
            this.listenTo(this.model, 'reset', this.reset);
        },
        initializeView: function(view) {
        },
        add: function(model) {
            var view = new this.view({
                model: model
            });
            this._views.push(view);
            this.initializeView(view);
            if(this._rendered) this.$el.append(view.render().el);
        },
        remove: function(model) {
            var viewToRemove = _(this._views).select(
                function(cv) { return cv.model === model; }
                )[0];
            this._views = _(this._views).without(viewToRemove);
            if(this._rendered) viewToRemove.$el.remove();
        },
        reset: function() {
            this._views = [];
            this._rendered = false;
            this.model.each(
                function(model) {
                    var view = new this.view({
                        model: model
                    });
                    this._views.push(view);
                    this.initializeView(view);
                    },
                this
                );
            this.render();
        },
        render: function() {
            this._rendered = true;
            this.$el.empty();
            _(this._views).each(
                function(dv) { this.$el.append(dv.render().el); },
                this
                );
            return this;
        }
    }
    );

