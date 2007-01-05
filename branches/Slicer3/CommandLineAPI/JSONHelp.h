#define JSONHelp "" \
"/* JSON version 1.0 */\n" \
"{\n" \
"        'class' : 'registration',\n" \
"        'name' : 'NAMIC Registration Command Line Demo',\n" \
"        'info' : 'Register two volumes',\n" \
"        'executable' : 'CLRegistration',\n" \
"        'parameters' : {\n" \
"                'histogrambins' : {\n" \
"                        'flag' : 'b',\n" \
"                        'info' : 'histogrambins',\n" \
"                        'description' : 'Number of histogram bins for MI',\n" \
"                        'type' : 'integer',\n" \
"                        'default' : 30,\n" \
"                        'constraints' : {\n" \
"                                      'range' : [ 1, 300, 50 ],\n" \
"                                      },\n" \
"                        },\n" \
"                'randomseed' : {\n" \
"                        'flag' : 'd',\n" \
"                        'info' : 'randomseed',\n" \
"                        'description' : 'Random seed for algorithm',\n" \
"                        'type' : 'string',\n" \
"                        'default' : 123456,\n" \
"                        },\n" \
"                'gradtolerance' : {\n" \
"                        'flag' : 'g',\n" \
"                        'info' : 'gradtolerance',\n" \
"                        'description' : 'Tolerance for optimizer',\n" \
"                        'type' : 'double',\n" \
"                        'default' : 0.0005,\n" \
"                        },\n" \
"                'iterations' : {\n" \
"                        'flag' : 'i',\n" \
"                        'info' : 'iterations',\n" \
"                        'description' : 'Number of iterations, may be comma separated list',\n" \
"                        'type' : 'string',\n" \
"                        'default' : '200',\n" \
"                        },\n" \
"                'learningrate' : {\n" \
"                        'flag' : 'l',\n" \
"                        'info' : 'learningrate',\n" \
"                        'description' : 'Optimizer Learning rate, may be comma separated list',\n" \
"                        'default' : '0.05',\n" \
"                        'type' : 'string',\n" \
"                        },\n" \
"                'spatialsamples' : {\n" \
"                        'flag' : 's',\n" \
"                        'info' : 'spatialsamples',\n" \
"                        'description' : 'Number of spatial samples for MI Metric',\n" \
"                        'default' : 10000,\n" \
"                        'type' : 'integer',\n" \
"                        'constraints' : {\n" \
"                                      'range' : [1000, 1000000, 1000],\n" \
"                                      },\n" \
"                        },\n" \
"                'translationscale' : {\n" \
"                        'flag' : 't',\n" \
"                        'info' : 'translationscale',\n" \
"                        'description' : 'Ratio of translations to rotations',\n" \
"                        'type' : 'double',\n" \
"                        'default' : '100.0',\n" \
"                        'constraints' : {\n" \
"                                      'range' : [10.0, 500.0, 5],\n" \
"                                      },\n" \
"                        },\n" \
"/*\n" \
"                'noinitializetransform' : {\n" \
"                        'flag' : 'u',\n" \
"                        'info' : 'noinitializetransform',\n" \
"                        \n" \
"                        },\n" \
"*/\n" \
"                'fixed' : {\n" \
"                        'type' : 'file',\n" \
"                        'required' : 'true',\n" \
"                        'channel' : 'input',\n" \
"                        'index' : 0,\n" \
"                        'description' : 'Fixed image for registration',\n" \
"                        'displayname' : 'Fixed Image',\n" \
"                        },\n" \
"                'moving' : {\n" \
"                        'type' : 'file',\n" \
"                        'required' : 'true',\n" \
"                        'channel' : 'input',\n" \
"                        'index' : 1,\n" \
"                        'description' : 'Moving image for registration',\n" \
"                        'displayname' : 'Moving Image',\n" \
"                        },\n" \
"                'registered' : {\n" \
"                        'type' : 'file',\n" \
"                        'required' : 'true',\n" \
"                        'channel' : 'output',\n" \
"                        'index' : 2,\n" \
"                        'description' : 'Resampled Moving image',\n" \
"                        'displayname' : 'Registered Image',\n" \
"                        },\n" \
"                },\n" \
"}\n"
