var api = require('../service/api');
var auth = require('../service/auth');
var ui = require('../ui');
var storage = require('../service/storage');
var MessageBox = require('../view/messagebox').MessageBox;
var PageView = require('../view/page').PageView;
var StaticView = require('../view/static').StaticView;

var SigninForm = StaticView.extend(
    {
        initialize: function() {
            StaticView.prototype.initialize.apply(this, arguments);
            this.render();
        },
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
            PageView.prototype.initialize.apply(this, arguments);
            this.form = new SigninForm;
            this.messageBox = new MessageBox;
            this.listenTo(
                this.form,
                'signin',
                this.signin.bind(this)
                );
            this.render();
        },
        signin: function(values) {
            api.rpcFunction('user_signin')(
                values.username, values.password
                ).then(
                    (function(user) {
                        auth.setToken(user['token']);
                        console.log('signed in', user);
                        storage.set('user', JSON.stringify(user));
                        auth.user().set(user);
                        this.messageBox.displaySuccess(
                            'You are now signed in as ' +
                            auth.user().get('username') + '.'
                            );
                        this.trigger('signedin');
                        this.render();
                    }).bind(this),
                    (function(error) {
                        this.messageBox.displayError(
                            'Sign in failed: ' + error
                            );
                    }).bind(this)
                    );
        },
        signedInMessage: function() {
            return p(
                    'You are already signed in as ' +
                    auth.user().get('username') + '.'
                    );
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Sign In'),
                    this.messageBox.el,
                    auth.user().isNew()?this.form.el:this.signedInMessage()
                   )
                );
        }
    }
    );

