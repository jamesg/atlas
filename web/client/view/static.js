var _ = require('underscore');
var domjs = require('domjs')(document);
var Backbone = require('backbone');
//Backbone.$ = require('jbone');

exports.StaticView = Backbone.View.extend(
    {
        initialize: function() {
            if(_.has(this, 'model'))
                this.listenTo(this.model, 'change', this.render.bind(this));
            this.render();
        },
        template: function() {
        },
        render: function() {
            //this.$el.empty();
            this.el.innerHTML = '';
            this.el.appendChild(domjs.build(this.template.bind(this)));
            return this;
        },
        setModel: function(model) {
            this.model = model;
            this.render();
        }
    }
    );

