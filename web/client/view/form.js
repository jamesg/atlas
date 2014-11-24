var _ = require('underscore');
var Q = require('q');
var ds = require('../service/ds');
var StaticView = require('./static').StaticView;

/*!
 * \brief An input and label displayed inline as part of a form.
 *
 * \param name Name of the input field and the attribute to set in the model.
 */
var InlineInput = StaticView.extend(
    {
        initialize: function(options) {
            StaticView.prototype.initialize.apply(this, arguments);
            this._label = ds.coalesce(options['label'], '');
            this._type = ds.coalesce(options['type'], 'text');
            this._name = options.name;
            this._placeholder = ds.coalesce(
                options['placeholder'],
                options['label'],
                ''
                );
            this._error = null;
            // Initial value.
            this._value = options.value;
            this.render();
        },
        name: function() {
            return this._name;
        },
        value: function() {
            if(this._type == 'checkbox')
                return this._input().checked;
            else
                return this._input().value;
        },
        setError: function(error) {
            this._error = error;
            this.render();
        },
        template: function() {
            if(this._type == 'checkbox')
                this._input = input(
                    {
                        type: this._type,
                        name: this._name,
                        checked: this._value
                    }
                    );
            else
                this._input = input(
                    {
                        type: this._type,
                        name: this._name,
                        placeholder: this._placeholder,
                        value: this._value
                    }
                    );
            return div(
                    {
                        class: 'pure-control-group ' +
                            ((this._error == null)?'group-valid':'group-invalid')
                    },
                    label({ for: this._name }, this._label),
                    this._input,
                    this._error
                    );
        }
    }
    );

exports.FormView = StaticView.extend(
    {
        initialize: function(options) {
            StaticView.prototype.initialize.apply(this, arguments);
            this._inputs = [];
        },
        /*!
         * \brief Create a simple text input field.
         *
         * \returns An instance of InlineInput (derivative of StaticView).
         */
        createInput: function(options) {
            var inputOptions = _(options).extend(
                {
                    value: this.model.get(options.name)
                }
                );
            var input_ = new InlineInput(inputOptions);
            this._inputs.push(input_);
            return input_;
        },
        // Read values from the form and set them in the model.
        _setAttrs: function() {
            _.each(
                this._inputs,
                function(input_) {
                    this.model.set(input_.name(), input_.value());
                },
                this
                );
        },
        save: function() {
            this._setAttrs();
            if(this.model.isValid())
            {
                _.each(
                    this._inputs,
                    function(input_) {
                        input_.setError(null);
                    }
                    );
                // Update the model on the server.
                return this.model.save().then(
                    (function(result) {
                        this.trigger('saved', this.model);
                        console.log('form save success');
                        return result;
                    }).bind(this),
                    (function(error) {
                        this.trigger('error', error);
                        console.log('form save error', error);
                        throw error;
                    }).bind(this)
                    );
            } else {
                // Transfer error strings to the form.
                var errors = this.model.validationError;
                _.each(
                    this._inputs,
                    function(input_) {
                        if(_.isObject(errors))
                            input_.setError(errors[input_.name()]);
                        else
                            input_.setError(null);
                    },
                    this
                    );
                var deferred = Q.defer();
                deferred.reject(errors);
                return deferred.promise;
            }
        }
    }
    );

