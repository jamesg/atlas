var domjs = require('domjs')(document);
var Backbone = require('backbone');

exports.StaticView = Backbone.View.extend(
    {
        initialize: function() {
            this.render();
        },
        template: function() {
        },
        render: function() {
            this.$el.empty();
            this.el.appendChild(domjs.build(this.template.bind(this)));
            return this;
        },
        setModel: function(model) {
            this.model = model;
            this.render();
        }
    }
    );

