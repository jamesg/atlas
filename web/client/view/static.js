var _ = require('underscore');
var domjs = require('domjs')(document);
var Backbone = require('backbone');

exports.StaticView = Backbone.View.extend(
    {
        initialize: function(options) {
            if(_.has(options, 'model'))
                this.model = options.model;
            if(_.has(this, 'model'))
                this.listenTo(this.model, 'change', this.render.bind(this));
        },
        reset: function() {
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

