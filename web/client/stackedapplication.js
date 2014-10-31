var _ = require('underscore');
var Backbone = require('backbone');
Backbone.$ = require('jbone');
var domjs = require('domjs')(document);

var Breadcrumb = require('./model/breadcrumb').Breadcrumb;
var BreadcrumbCollection = require('./collection/breadcrumb').BreadcrumbCollection;
var HomePage = require('./page/home').HomePage;
var Navigation = require('./view/navigation').Navigation;
var AuthenticationRequiredPage =
        require('./page/authenticationrequired').AuthenticationRequiredPage;

exports.StackedApplication = function() {
    this.breadcrumbs = new BreadcrumbCollection;
    this.navigation = new Navigation({ application: this, breadcrumbs: this.breadcrumbs });
    document.getElementById('template_header')
        .appendChild(this.navigation.render().el);

    this.goHome();
};

_.extend(exports.StackedApplication, Backbone.Events);

exports.StackedApplication.prototype._setElement = function(el) {
    document.getElementById('template_content').innerHTML = '';
    document.getElementById('template_content').appendChild(el);
};

exports.StackedApplication.prototype._createPage = function(constructor) {
    return (_.isFunction(constructor))?
        (new constructor({ application: this })):
        constructor;
};

exports.StackedApplication.prototype._createBreadcrumb = function(view) {
    return new Breadcrumb(
        {
            page_title: _.isFunction(view.pageTitle)?
                    view.pageTitle():view.pageTitle,
            view: view
        }
        );
};

exports.StackedApplication.prototype._setPage = function(view) {
    if(view.fullPage)
        this.navigation.deactivate();
    else
        this.navigation.activate();

    this._setElement(view.el);
};

exports.StackedApplication.prototype.goHome = function() {
    this.breadcrumbs.reset();
    var home = this._createPage(HomePage);
    this.breadcrumbs.add(this._createBreadcrumb(home));
    this._setPage(home);
};

exports.StackedApplication.prototype.gotoPage = function(constructor) {
    this.breadcrumbs.reset();
    var home = this._createPage(HomePage);
    this.breadcrumbs.add(this._createBreadcrumb(home));
    this.pushPage(constructor);
};

exports.StackedApplication.prototype.pushPage = function(constructor) {
    var view = this._createPage(constructor);
    this.breadcrumbs.add(this._createBreadcrumb(view));
    this._setPage(view);
};

exports.StackedApplication.prototype.setRoute = function() {
    this.breadcrumbs.reset();
    for(c in arguments)
        this.pushPage(arguments[c]);
};

exports.StackedApplication.prototype.popPage = function() {
    this.breadcrumbs.pop();
    if(this.breadcrumbs.length == 0)
        this.goHome();
    else
        this._setPage(this.breadcrumbs.at(this.breadcrumbs.length-1).get('view'));
};

exports.StackedApplication.prototype.currentPage = function() {
    // warning: only valid if there is at least one page.
    return this.breadcrumbs.at(this.breadcrumbs.length-1).get('view');
};

exports.StackedApplication.prototype.revisit = function(breadcrumb) {
    var i = 0;
    while(i < this.breadcrumbs.length) {
        if(this.breadcrumbs.at(i) == breadcrumb) {
            this.breadcrumbs.remove(this.breadcrumbs.slice(i+1, this.breadcrumbs.length));
            this._setPage(this.breadcrumbs.at(i).get('view'));
            this.currentPage().reset();
            return;
        }
        ++i;
    }
};

/*!
 * \brief Handle an authentication error by displaying a sign in page over the
 * current page.  Will only display a sign in page if none is currently
 * displayed.
 */
exports.StackedApplication.prototype.authenticationError = function() {
    if(!(this.currentPage() instanceof AuthenticationRequiredPage)) {
        this.pushPage(AuthenticationRequiredPage);
    }
};

