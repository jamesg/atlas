var _ = require('underscore');
var StaticView = require('./static').StaticView;

exports.PageView = StaticView.extend(
    {
        pageTitle: 'Untitled Page',
        fullPage: false,
        initialize: function(options) {
            if(_.has(options, 'application'))
                this.application = options.application;
            StaticView.prototype.initialize.apply(this, arguments);
        },
        reset: function() {
            console.log('reset page');
            this.render();
        }
    }
    );

