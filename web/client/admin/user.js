var _ = require('underscore');
var ds = require('../service/ds');
var FormView = require('../view/form').FormView;
var MessageBox = require('../view/messagebox').MessageBox;
var PageView = require('../view/page').PageView;
var StaticView = require('../view/static').StaticView;
var User = require('../model/user').User;
var ui = require('../ui');

var UserForm = FormView.extend(
    {
        initialize: function() {
            FormView.prototype.initialize.apply(this, arguments);
            this._messageBox = new MessageBox;
            this._username = this.createInput(
                {
                    name: 'username',
                    label: 'Username'
                }
                );
            this._enabled = this.createInput(
                {
                    type: 'checkbox',
                    name: 'user_enabled',
                    label: 'Account enabled'
                }
                );
            this._super = this.createInput(
                {
                    type: 'checkbox',
                    name: 'user_super',
                    label: 'Superuser'
                }
                );
            this.render();
        },
        setModel: function(model) {
            this.model = model;
            this.render();
        },
        template: function() {
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
                    class: 'pure-form pure-form-aligned',
                    onsubmit: (function() {
                        this.save().then(
                            (function() {
                                this._messageBox.displaySuccess('User details saved');
                            }).bind(this),
                            (function(errors) {
                                this._messageBox.displayError(
                                    ds.errorString(errors)
                                    );
                            }).bind(this)
                            );
                        return false;
                    }).bind(this)
                },
                this._messageBox.el,
                fieldset(this._username.el, this._enabled.el, this._super.el),
                buttonBox
                );
        }
    }
    );

exports.UserAdmin = PageView.extend(
    {
        pageTitle: function() {
            return this.model.get('username');
        },
        initialize: function(options) {
            PageView.prototype.initialize.apply(this, arguments);
            if(!_.has(this, 'model'))
                this.model = new User;
            this._form = new UserForm({ model: this.model });

            this.listenTo(
                this._form,
                'saved',
                this.application.popPage.bind(this.application)
                );

            this.render();
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('User'),
                    this._form.el
                   )
                );
        }
    }
    );

