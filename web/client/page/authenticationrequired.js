var SigninPage = require('../page/signin').SigninPage;

/*!
 * \brief A sign in page displayed when the user has attempted to access
 * information or perform an action that they are not privileged to.
 */
exports.AuthenticationRequiredPage = SigninPage.extend(
    {
        pageTitle: 'Authentication Required',
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Authentication Required'),
                    p(
                        'You have accessed a page requiring greater ' +
                        'privileges than your current profile allows.  ' +
                        'Please sign in using an account with sufficient ' +
                        'privileges.'
                        ),
                    this.form.el
                   )
                );
        }
    }
    );

