var auth = require('../service/auth');
var SigninPage = require('../page/signin').SigninPage;

/*!
 * \brief A sign in page displayed when the user has volunteered to sign in.
 */
exports.NewsigninPage = SigninPage.extend(
    {
        initialize: function() {
            SigninPage.prototype.initialize.apply(this, arguments);
            this.render();
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Sign In'),
                    this.messageBox.el,
                    p(
                        'Please enter a valid username and password.'
                        ),
                    auth.user().isNew()?this.form.el:this.signedInMessage()
                   )
                );
        }
    }
    );

