var SigninPage = require('../page/signin').SigninPage;

/*!
 * \brief A sign in page displayed when the user has volunteered to sign in.
 */
exports.NewsigninPage = SigninPage.extend(
    {
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Sign In'),
                    p(
                        'Please enter a valid username and password.'
                        ),
                    this.form.el
                   )
                );
        }
    }
    );

