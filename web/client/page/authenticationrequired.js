var auth = require('../service/auth');
var SigninPage = require('../page/signin').SigninPage;

/*!
 * \brief A sign in page displayed when the user has attempted to access
 * information or perform an action that they are not privileged to.
 */
exports.AuthenticationRequiredPage = SigninPage.extend(
    {
        pageTitle: 'Authentication Required',
        initialize: function() {
            SigninPage.prototype.initialize.apply(this, arguments);
            this.listenTo(
                this,
                'signedin',
                this.application.popPage.bind(this.application)
                );
            this.render();
        },
        newsessionTemplate: function() {
            return p(
                'You have accessed a page which requires you to sign in.  ' +
                'Please sign in to continue.'
                );
        },
        reauthTemplate: function() {
            return p(
                'You are currently signed in as ' +
                auth.user().get('username') + ', but your account does not ' +
                'have sufficient privileges to access this page.  Please ' +
                'sign in with a more privileged account.'
                );
        },
        template: function() {
            return div(
                { class: 'pure-g' },
                div(
                    { class: 'pure-u-1-1' },
                    h2('Authentication Required'),
                    this.messageBox.el,
                    auth.user().isNew()?
                        this.newsessionTemplate():
                        this.reauthTemplate(),
                    this.form.el
                   )
                );
        }
    }
    );

