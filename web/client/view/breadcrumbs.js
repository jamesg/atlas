var BreadcrumbCollection = require('../collection/breadcrumb').BreadcrumbCollection;
var CollectionView = require('../view/collection').CollectionView;
var StaticView = require('./static').StaticView;
var icon = require('../ui').icon;

var BreadcrumbView = StaticView.extend(
    {
        tagName: 'span',
        template: function() {
            span(' '), icon('caret-right'), span(' '),
            button(
                {
                    class: 'display-link',
                    onclick: (function() {
                        this.application.revisit(this.model);
                    }).bind(this)
                },
                this.model.get('page_title')
                );
        }
    }
    );

exports.BreadcrumbsView = CollectionView.extend(
    {
        view: BreadcrumbView,
        initialize: function(options) {
            this.application = options.application;
            CollectionView.prototype.initialize.apply(this, arguments);
        },
        initializeView: function(view) {
            view.application = this.application;
            view.render();
        },
        tagName: 'ul'
    }
    );

