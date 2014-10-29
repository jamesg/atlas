var _ = require('underscore');
var Q = require('q');

var hasLocalStorage = (function() {
    console.log('testing localStorage');
    try {
        localStorage.setItem(mod, mod);
        localStorage.removeItem(mod);
        console.log('found localStorage');
        return true;
    } catch(exception) {
        console.log('did not find localStorage');
        return false;
    }
}());

var getStorage = function(key) {
    if(hasLocalStorage)
        return window.localStorage.get(key);
    else
        return gStorage[key];
};

var setStorage = function(key, value) {
    if(hasLocalStorage)
        window.localStorage.setItem(key, value);
    else
        gStorage[key] = value;
};

/*!
 * \brief Set the authentication token to be used in all future API requests.
 */
exports.setToken = function(token) {
    setStorage('token', token);
};

/*!
 * \brief Make a JSON remote procedure call.
 *
 * \param options Map of JSONRPC request options.  Keys are method, params,
 * xhr.
 *
 * \returns A Q promise.
 */
exports.rpc = function(options) {
    var deferred = Q.defer();

    var req = _.has(options, 'xhr')?options.xhr:new XMLHttpRequest;
    var reqListener = function() {
        console.log('api response: ' + this.responseText);
        var jsonIn = JSON.parse(this.responseText);
        if(_.has(jsonIn, 'result'))
            deferred.resolve(jsonIn);
        else
            deferred.reject(jsonIn);
    }

    var requestContent = JSON.stringify(_.pick(options, 'method', 'params'));

    console.log('api request: ' + requestContent);

    req.open('post', '/api_call', true);

    req.setRequestHeader('Authorization', getStorage('auth_token'));
    req.onload = reqListener;
    req.send(requestContent);

    return deferred.promise.then(
            function(jsonIn) {
                return jsonIn['result'];
            },
            function(jsonIn) {
                if(_.has(jsonIn, 'unauthorised') && jsonIn.unauthorised) {
                    gApplication.authenticationError();
                }
                throw jsonIn['error'];
            }
            );
};

exports.rpcFunction = function(api_function) {
    return function() {
        return exports.rpc({
            method: _.flatten(
                _.map(api_function, function(c) {
                    return (/[A-Z]/.test(c))?('_' + c.toLowerCase()):c;
                })
                ).join(''),
            params: _.toArray(arguments)
        });
    };
};

/*!
 * \brief Generate a function that can be used as a Backbone model sync function.
 *
 * The resulting function can be used for specialisations of Backbone.Model and
 * of Backbone.Collection.
 *
 * \param rpcFunctions Map of Backbone methods to RPC function names.  Keys are
 * create, read, update and remove.
 */
exports.backboneSyncFunction = function(rpcFunctions) {
    return function sync(method, model, options) {
        if(_.has(rpcFunctions, method))
        {
            var function_ = _.isFunction(rpcFunctions[method])?
                rpcFunctions[method]:
                _.partial(
                        exports.rpc,
                        {
                            method: rpcFunctions[method],
                            params: [model.toJSON()]
                        }
                        );
            return function_().then(
                function(data) {
                    console.log('sync success', data);
                    options.success(data);
                    return data;
                },
                function(err) {
                    console.log('sync error', err);
                    options.error(err);
                    return err;
                }
                );
        }
        else
            throw 'no function for method ' + method;
    }
};

