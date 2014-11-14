var _ = require('underscore');
var Backbone = require('backbone');
var PageView = require('../view/page').PageView;
var FormView = require('../view/form').FormView;
var auth = require('../service/auth');
var ui = require('../ui');

var ChangePassword = Backbone.Model.extend(
    {
        defaults: {
            current: '',
            password: '',
            repeat: ''
        },
        validate: function(attrs) {
            var errors = {};
            if(attrs['password'].length < 6)
                errors['password'] = 'New password must be at least 6 characters';
            if(attrs['password'] != attrs['repeat'])
                errors['password'] = errors['repeat'] =
                    'New passwords must match.';
            if(!_.isEmpty(errors))
                return errors;
        }
    }
    );

var ChangePasswordForm = FormView.extend(
        {
            initialize: function() {
                FormView.prototype.initialize.apply(this, arguments);
                this.model = new ChangePassword;
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
                        onclick: (function() {
                            return false;
                        }).bind(this)
                    },
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

