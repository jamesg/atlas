var domjs = require('domjs')(document);
var _ = require('underscore');
var Backbone = require('backbone');

/*!
 * \brief A basic table row (tr) view representing a single model within a
 * table.  Extend this view to implement a table row for a particular model.
 *
 * \note By default, the table is 'editable' (rows are highlighted when the
 * mouse cursor is placed above them).  To disable this behaviour, change the
 * className attribute in derived views.
 */
exports.TrView = Backbone.View.extend(
    {
        tagName: 'tr',
        className: 'editable-tr',
        initialize: function() {
            this.render();
            this.$el.on('click', this.trigger.bind(this, 'click', this.model));
            this.listenTo(this.model, 'change', this.render);
        },
        template: function() {
        },
        render: function() {
            this.$el.empty();
            this.$el.append(domjs.build(this.template.bind(this)));
            return this;
        }
    }
    );

