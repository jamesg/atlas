var _ = require('underscore');
var Backbone = require('backbone');

var api = require('../service/api');

exports.User = Backbone.Model.extend(
    {
        initialize: function() {
            Backbone.Model.prototype.initialize.apply(this, arguments);
            console.log('initialize user model');
        },
        set: function(attributes) {
            Backbone.Model.prototype.set.apply(this, arguments);
            console.log('set', attributes);
        },
        defaults: {
            username: ''
        },
        idAttribute: 'user_id',
        sync: api.backboneSyncFunction(
            {
                create: 'user_save',
                delete: 'user_destroy',
                read: 'user_get',
                update: 'user_save'
            }
            ),
        validate: function(attrs) {
            if(attrs['username'] == 'root')
                return 'The root account cannot be edited';

            var errors = {};
            if(attrs['username'] == '')
                errors['username'] = 'Name is required';
            if(attrs['description'] == '')
                errors['description'] = 'Description is required';
            if(!_.isEmpty(errors))
                return errors;
        }
    }
    );

