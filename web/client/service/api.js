var _ = require('underscore');
var Q = require('q');
var storage = require('./storage');

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
        var jsonIn = JSON.parse(this.responseText);
        if(_.has(jsonIn, 'result'))
            deferred.resolve(jsonIn);
        else
            deferred.reject(jsonIn);
    }

    options.token = storage.get('token');
    var requestContent = JSON.stringify(options);//_.pick(options, 'method', 'params', 'token'));


    req.open('post', '/api_call', true);

    req.setRequestHeader('Authorization', storage.get('token'));
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
            var promise = function_();
            if(promise)
                return promise.then(
                    function(data) {
                        console.log('sync success', data);
                        options.success(data);
                        return data;
                    },
                    function(err) {
                        console.log('sync error', err, rpcFunctions[method], [model.toJSON()]);
                        options.error(err);
                        throw err;
                    }
                    );
        }
        else
            throw 'no function for method ' + method;
    }
};

