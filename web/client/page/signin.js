var MessageBox = require('../view/messagebox').MessageBox;
var PageView = require('../view/page').PageView;
var StaticView = require('../view/static').StaticView;
var api = require('../service/api');
var ui = require('../ui');

var SigninForm = StaticView.extend(
    {
        template: function() {
            var username = input({ type: 'text', placeholder: 'Username' });
            var password = input({ type: 'password', placeholder: 'Password' });
            return form(
                {
                    class: 'pure-form pure-form-aligned',
                    onsubmit: (function() {
                        this.trigger(
                            'signin',
                            {
                                username: username().value,
                                password: password().value
                            }
                            );
                        return false;
                    }).bind(this)
                },
                username,
                password,
                button(
                    {
                        class: 'pure-button pure-button-primary',
                        type: 'submit'
                    },
                    ui.icon('person'), 'Sign In'
                    )
                );
        }
    }
    );

/*!
 * \brief A basic sign in page.
 */
exports.SigninPage = PageView.extend(
    {
        pageTitle: 'Sign In',
        initialize: function() {
            this.form = new SigninForm;
            this._messageBox = new MessageBox;
            this.listenTo(
                this.form,
                'signin',
                this.signin.bind(this)
                );
            PageView.prototype.initialize.apply(this, arguments);
        },
        signin: function(auth) {
            api.rpcFunction('user_signin')(
                auth.username, auth.password
                ).then(
                    (function(token) {
                        api.setToken(token);
                        this.application.popPage();
                        this.application.currentPage().reset();
                    }).bind(this),
                    (function(error) {
                        this._messageBox.displayError(
                            'Sign in failed: ' + error
                            );
                    }).bind(this)
                    );
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Sign In'),
                    this.form.el
                   )
                );
        }
    }
    );

