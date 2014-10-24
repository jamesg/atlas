var domjs = require('domjs')(document);
var _ = require('underscore');
var Backbone = require('backbone');
Backbone.$ = require('jbone');

var CollectionView = require('../view/collection').CollectionView;

var TbodyView = CollectionView.extend(
    {
        tagName: 'tbody',
        initialize: function(options) {
            this.initializeTrView = options.initializeTrView;
            CollectionView.prototype.initialize.apply(this, arguments);
        },
        initializeView: function(view) {
            this.listenTo(view, 'click', this.trigger.bind(this, 'click'));
            this.initializeTrView(view);
        }
    }
    );

/*!
 * \brief A generic table view that can be adapted to any kind of table with a
 * header.  Provide a view constructor for the table header (thead) as the
 * theadView option and a view constructor for the table rows (tr) as the
 * trView option.
 */
exports.TableView = Backbone.View.extend(
    {
        tagName: 'table',
        className: 'pure-table',
        initialize: function(options) {
            if(_.has(options, 'model'))
                this.model = options.model;
            if(_.has(options, 'theadView'))
                this.theadView = options.theadView;
            if(_.has(options, 'trView'))
                this.trView = options.trView;
            if(_.has(options, 'initializeTrView'))
                this.initializeTrView = options.initializeTrView;

            this._thead = new this.theadView;
            this._thead.render();
            this._tbody = new TbodyView(
                {
                    initializeTrView: this.initializeTrView,
                    view: this.trView,
                    model: this.model
                }
                );
            this._tbody.render();

            this.listenTo(this.model, 'all', this.render);
            this.listenTo(this._tbody, 'click', this.trigger.bind(this, 'click'));

            this.render();
        },
        initializeTrView: function(trView) {
        },
        render: function() {
            this.$el.empty();
            this.$el.append(this._thead.el);
            this.$el.append(this._tbody.el);
            return this;
        }
    }
    );

