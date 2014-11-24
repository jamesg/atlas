var _ = require('underscore');

var hasLocalStorage = (function() {
    console.log('testing localStorage');
    try {
        localStorage.setItem('mod', 'mod');
        localStorage.removeItem('mod');
        console.log('found localStorage');
        return true;
    } catch(exception) {
        console.log('did not find localStorage');
        return false;
    }
}());

if(hasLocalStorage)
    exports.get = function(key) {
        return window.localStorage.getItem(key);
    };
else
    exports.get = function(key) {
        return gStorage[key];
    };

if(hasLocalStorage)
    exports.set = function(key, value) {
        window.localStorage.setItem(key, value);
    };
else
    exports.set = function(key, value) {
        gStorage[key] = value;
    };

if(hasLocalStorage)
    exports.remove = function(key) {
        window.localStorage.setItem(key, null);
    };
else
    exports.remove = function(key) {
        delete gStorage['key'];
    };

if(hasLocalStorage)
    exports.has = function(key) {
        return (window.localStorage.getItem(key) != null);
    };
else
    exports.has = function(key) {
        return _.has(gStorage, key);
    };

