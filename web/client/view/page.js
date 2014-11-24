var _ = require('underscore');
var StaticView = require('./static').StaticView;

exports.PageView = StaticView.extend(
    {
        pageTitle: 'Untitled Page',
        fullPage: false,
        initialize: function(options) {
            StaticView.prototype.initialize.apply(this, arguments);
            if(_.has(options, 'application'))
                this.application = options.application;
        }
    }
    );

