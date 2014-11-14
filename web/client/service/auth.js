var _ = require('underscore');
var storage = require('./storage');
var User = require('../model/user').User;

var gUser = (function() {
    console.log('init user', storage.has('user'));
    if(storage.has('user'))
        return new User(JSON.parse(storage.get('user')));
    else
        return new User;
})();

/*!
 * \brief Get the global user model.  If the user is not signed in, isNew() on
 * the resulting model will be true.
 */
exports.user = function() {
    return gUser;
};

exports.isSignedIn = function() {
    return gUser.isNew();
};

exports.signOut = function() {
    storage.remove('token');
    storage.remove('user');
    gUser.clear();
};

/*!
 * \brief Set the authentication token to be used in all future API requests.
 */
exports.setToken = function(token) {
    storage.set('token', token);
};

/*!
 * \brief Get the authentication token for an API request.
 */
exports.getToken = function() {
    return storage.get('token');
};

