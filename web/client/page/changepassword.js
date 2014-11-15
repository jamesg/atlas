var _ = require('underscore');
var Backbone = require('backbone');
var PageView = require('../view/page').PageView;
var FormView = require('../view/form').FormView;
var MessageBox = require('../view/messagebox').MessageBox;
var api = require('../service/api');
var auth = require('../service/auth');
var ui = require('../ui');

var ChangePassword = Backbone.Model.extend(
    {
        idAttribute: 'user_id',
        defaults: {
            current: '',
            password: '',
            repeat: ''
        },
        sync: api.backboneSyncFunction({ update: 'user_change_password' }),
        validate: function(attrs) {
            var errors = {};
            if(attrs['password'] != attrs['repeat'])
                errors['password'] = errors['repeat'] =
                    'New passwords must match.';
            if(attrs['password'].length < 6)
                errors['password'] = 'New password must be at least 6 characters';
            if(!_.isEmpty(errors))
                return errors;
        }
    }
    );

var ChangePasswordForm = FormView.extend(
        {
            initialize: function() {
                FormView.prototype.initialize.apply(this, arguments);
                this._messageBox = new MessageBox;
                this.model = new ChangePassword;
                this.model.set('user_id', auth.user().get('user_id'));
                console.log('user_id', this.model.get('user_id'));
                this._currentPassword = this.createInput(
                    {
                        type: 'password',
                        name: 'current',
                        label: 'Current Password'
                    }
                    );
                this._newPassword = this.createInput(
                    {
                        type: 'password',
                        name: 'password',
                        label: 'New Password'
                    }
                    );
                this._repeatPassword = this.createInput(
                    {
                        type: 'password',
                        name: 'repeat',
                        label: 'Repeat Password'
                    }
                    );
                this.render();
            },
            template: function() {
                return form(
                    {
                        class: 'pure-form pure-form-aligned',
                        onsubmit: (function() {
                            this.save().then(
                                (function(site) {
                                    this._messageBox.displaySuccess('Password changed.');
                                }).bind(this),
                                (function(errors) {
                                    this._messageBox.displayError(
                                        JSON.stringify(errors)
                                        );
                                }).bind(this)
                                );
                            return false;
                        }).bind(this)
                    },
                    this._messageBox.el,
                    legend(
                        'Please enter your current password and a new password'
                        ),
                    fieldset(
                        this._currentPassword.el,
                        this._newPassword.el,
                        this._repeatPassword.el,
                        ui.inlineInput('', ui.saveButton())
                        )
                    );
            }
        }
        );

/*!
 * \brief Allow a user to change their password, or allow a superuser to change
 * the password of another user.
 */
exports.ChangePasswordPage = PageView.extend(
        {
            pageTitle: 'Change Password',
            initialize: function() {
                PageView.prototype.initialize.apply(this, arguments);
                if(!_.has(this, 'model'))
                    this.model = auth.user();
                this._form = new ChangePasswordForm;

                // TODO: require that the user is signed in.
                if(this.model.get('user_id') != auth.user().get('user_id'))
                {
                    // TODO: require that the user is a superuser.
                }
                this.render();
            },
            template: function() {
                return div(
                    { class: 'pure-g' },
                    div(
                        { class: 'pure-u-1-1' },
                        h2('Change Password'),
                        this._form.el
                       )
                    );
            }
        }
        );

