var Backbone = require('backbone');
var Breadcrumb = require('../model/breadcrumb').Breadcrumb;

exports.BreadcrumbCollection = Backbone.Collection.extend(
    {
        model: Breadcrumb
    }
    );

