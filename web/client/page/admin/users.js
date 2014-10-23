var _ = require('underscore');

var ui = require('../../ui');

var User = require('../../model/user').User;
var UserCollection = require('../../collection/user').UserCollection;
var MessageBox = require('../../view/messagebox').MessageBox;
var PageView = require('../../view/page').PageView;
var StaticView = require('../../view/static').StaticView;
var TableView = require('../../view/table').TableView;
var TrView = require('../../view/tr').TrView;

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

var UserForm = StaticView.extend(
    {
        initialize: function() {
            this._messageBox = new MessageBox;
            this.render();
        },
        setModel: function(model) {
            this.model = model;
            this.render();
        },
        template: function() {
            var username = input({ type: 'text', value: this.model.get('username') });
            var enabled = input({ type: 'checkbox', value: this.model.get('user_enabled') });
            var super_ = input({ type: 'checkbox', value: this.model.get('user_super') });
            var buttonBox = div(
                    { class: 'buttonbox' },
                    button(
                        {
                            type: 'button',
                            class: 'pure-button',
                            onclick: (function() {
                                this.setModel(new User);
                            }).bind(this)
                        },
                        ui.icon('cross'), 'Clear'
                        )
                    );
            if(!this.model.isNew())
            {
                buttonBox(
                    ui.confirmButton(
                        _.partial(span, ui.icon('delete'), 'Delete'),
                        (function() {
                            this.model.destroy();
                            this.setModel(new User);
                            this.trigger('saved');
                        }).bind(this)
                        )
                    );
            }
            buttonBox(
                button(
                    {
                        type: 'submit',
                        class: 'pure-button pure-button-primary'
                    },
                    ui.icon('data-transfer-download'), (this.model.isNew()?'Create':'Update')
                    )
                );
            return form(
                {
                    class: 'pure-form pure-form-stacked',
                    onsubmit: (function() {
                        this.model.set('username', username().value);
                        this.model.set('user_enabled', (enabled().value=='value'));
                        this.model.set('user_super', (super_().value=='value'));
                        if(this.model.isValid()) {
                            this.model.save().then(
                                (function() {
                                    this._messageBox.displaySuccess('User details saved');
                                    this.trigger('saved');
                                }).bind(this)
                                );
                        } else {
                            this._messageBox.displayError(
                                ds.errorString(this.model.validationError)
                                );
                        }
                        return false;
                    }).bind(this)
                },
                this._messageBox.el,
                fieldset(
                    label('Username', username),
                    label(enabled, 'Account enabled'),
                    label(super_, 'Superuser')
                    ),
                buttonBox
                );
        }
    }
    );

var UserPage = PageView.extend(
    {
        pageTitle: function() {
            return 'Edit \'' + this.model.get('username') + '\'';
        },
        function initialize(options) {
            if(_.has(options, 'model'))
                this.model = options.model;
            if(!_.has(this, 'model'))
                this.model = new User;
            this.form = new UserForm({ model: this.model });
            this.listenTo(
                this.form,
                'saved',
                this.users.fetch.bind(this.users)
                );
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Edit User'),
                    this.form
                   )
                );
        }
    }
    );

exports.UsersPage = StaticView.extend(
    {
        initialize: function() {
            this.users = new UserCollection;
            this.users.fetch();
            this.usersTable = new UsersTable({ model: this.users });
            this.listenTo(
                this.usersTable,
                'click',
                this.form.setModel.bind(this.form)
                );
            this.render();
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Users'),
                    div(
                        { class: 'pure-g' },
                        div({ class: 'pure-u-12-24' }, this.usersTable.el),
                        div({ class: 'pure-u-12-24' }, this.form.el)
                        )
                   )
                );
        }
    }
    );

