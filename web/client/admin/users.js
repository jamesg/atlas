var _ = require('underscore');
var ui = require('../ui');
var PageView = require('../view/page').PageView;
var StaticView = require('../view/static').StaticView;
var TableView = require('../view/table').TableView;
var TrView = require('../view/tr').TrView;
var User = require('../model/user').User;
var UserAdmin = require('../admin/user').UserAdmin;
var UserCollection = require('../collection/user').UserCollection;

var UsersTable = TableView.extend(
    {
        theadView: StaticView.extend(
            {
                tagName: 'thead',
                template: function() {
                    return tr(
                        th('Username'),
                        th('Enabled'),
                        th('Superuser')
                        );
                }
            }
            ),
        trView: TrView.extend(
            {
                template: function() {
                    td(this.model.get('username'));
                    td(this.model.get('user_enabled')?ui.icon('check'):ui.icon('x'));
                    td(this.model.get('user_super')?ui.icon('check'):ui.icon('x'));
                }
            }
            )
    }
    );

exports.UsersAdmin = PageView.extend(
    {
        pageTitle: 'Users',
        initialize: function() {
            this.users = new UserCollection;
            this.users.fetch();
            this.usersTable = new UsersTable({ model: this.users });
            this.listenTo(
                this.usersTable,
                'click',
                this.gotoUser.bind(this)
                );
            PageView.prototype.initialize.apply(this, arguments);
        },
        reset: function() {
            console.log('reset users page');
            this.users.fetch();
        },
        gotoNewUser: function() {
            this.application.pushPage(UserAdmin);
        },
        gotoUser: function(user) {
            this.application.pushPage(
                new UserAdmin(
                    {
                        application: this.application,
                        model: user
                    }
                    )
                );
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Users'),
                    div(
                        { class: 'buttonbox' },
                        button(
                            {
                                class: 'pure-button button-success',
                                onclick: this.gotoNewUser.bind(this)
                            },
                            span(ui.icon('plus'), 'New User')
                            )
                        ),
                    this.usersTable.el
                   )
                );
        }
    }
    );

