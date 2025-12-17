{
    "patcher": {
        "fileversion": 1,
        "appversion": {
            "major": 9,
            "minor": 1,
            "revision": 1,
            "architecture": "x64",
            "modernui": 1
        },
        "classnamespace": "box",
        "rect": [ 636.0, 468.0, 847.0, 544.0 ],
        "boxes": [
            {
                "box": {
                    "attr": "normalize",
                    "id": "obj-4",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 33.0, 169.76607578992844, 192.0, 22.0 ]
                }
            },
            {
                "box": {
                    "bgcolor": [ 0.12549019607843137, 0.12549019607843137, 0.12549019607843137, 1.0 ],
                    "fontsize": 13.0,
                    "id": "obj-27",
                    "linecount": 4,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 576.0, 137.76607578992844, 81.28654617071152, 66.0 ],
                    "text": "Audio file\nSine wave\nWhite noise\nAudio input",
                    "textcolor": [ 0.6196078431372549, 0.7450980392156863, 0.9529411764705882, 1.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-26",
                    "maxclass": "gain~",
                    "multichannelvariant": 0,
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 644.0, 352.76607578992844, 123.0, 26.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-25",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "signal" ],
                    "patching_rect": [ 703.0, 217.76607578992844, 35.0, 22.0 ],
                    "text": "adc~"
                }
            },
            {
                "box": {
                    "hidden": 1,
                    "id": "obj-23",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "int" ],
                    "patching_rect": [ 559.0, 217.76607578992844, 29.5, 22.0 ],
                    "text": "+ 1"
                }
            },
            {
                "box": {
                    "disabled": [ 0, 0, 0, 0 ],
                    "id": "obj-22",
                    "itemtype": 0,
                    "maxclass": "radiogroup",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 559.0, 137.76607578992844, 18.0, 66.0 ],
                    "size": 4,
                    "value": 0
                }
            },
            {
                "box": {
                    "hidden": 1,
                    "id": "obj-17",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "patching_rect": [ 59.0, 273.0, 58.0, 22.0 ],
                    "text": "loadbang"
                }
            },
            {
                "box": {
                    "id": "obj-16",
                    "maxclass": "dict.view",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 33.0, 353.0, 237.0, 125.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-15",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 5,
                    "outlettype": [ "dictionary", "", "", "", "" ],
                    "patching_rect": [ 33.0, 312.0, 61.0, 22.0 ],
                    "saved_object_attributes": {
                        "legacy": 0,
                        "parameter_enable": 0,
                        "parameter_mappable": 0
                    },
                    "text": "dict"
                }
            },
            {
                "box": {
                    "id": "obj-13",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 644.0, 432.76607578992844, 45.0, 45.0 ]
                }
            },
            {
                "box": {
                    "attr": "overlap",
                    "id": "obj-12",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 33.0, 193.76607578992844, 192.0, 22.0 ]
                }
            },
            {
                "box": {
                    "attr": "dict",
                    "id": "obj-11",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 33.0, 145.76607578992844, 192.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-10",
                    "maxclass": "newobj",
                    "numinlets": 5,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 644.0, 267.76607578992844, 78.0, 22.0 ],
                    "text": "selector~ 4 1"
                }
            },
            {
                "box": {
                    "id": "obj-9",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 674.0, 169.76607578992844, 66.0, 22.0 ],
                    "text": "cycle~ 440"
                }
            },
            {
                "box": {
                    "id": "obj-8",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 688.0, 193.76607578992844, 44.0, 22.0 ],
                    "text": "noise~"
                }
            },
            {
                "box": {
                    "data": {
                        "clips": [
                            {
                                "absolutepath": "sample.wav",
                                "filename": "sample.wav",
                                "filekind": "audiofile",
                                "id": "u155000864",
                                "selection": [ 0.0, 1.0 ],
                                "loop": 1,
                                "content_state": {
                                    "loop": 1
                                }
                            }
                        ]
                    },
                    "id": "obj-7",
                    "maxclass": "playlist~",
                    "mode": "basic",
                    "numinlets": 1,
                    "numoutlets": 5,
                    "outlettype": [ "signal", "signal", "signal", "", "dictionary" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 659.0, 137.76607578992844, 150.0, 30.0 ],
                    "quality": "basic",
                    "saved_attribute_attributes": {
                        "candicane2": {
                            "expression": ""
                        },
                        "candicane3": {
                            "expression": ""
                        },
                        "candicane4": {
                            "expression": ""
                        },
                        "candicane5": {
                            "expression": ""
                        },
                        "candicane6": {
                            "expression": ""
                        },
                        "candicane7": {
                            "expression": ""
                        },
                        "candicane8": {
                            "expression": ""
                        }
                    }
                }
            },
            {
                "box": {
                    "attr": "buffer_size",
                    "id": "obj-14",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 33.0, 121.76607578992844, 192.0, 22.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-6",
                    "maxclass": "live.line",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 493.0, 805.0, 5.0 ]
                }
            },
            {
                "box": {
                    "bgcolor": [ 0.301961, 0.301961, 0.301961, 0.0 ],
                    "bgcolor2": [ 0.301961, 0.301961, 0.301961, 0.0 ],
                    "bgfillcolor_angle": 270.0,
                    "bgfillcolor_autogradient": 0.0,
                    "bgfillcolor_color": [ 0.2, 0.2, 0.2, 0.0 ],
                    "bgfillcolor_color1": [ 0.301961, 0.301961, 0.301961, 0.0 ],
                    "bgfillcolor_color2": [ 0.2, 0.2, 0.2, 1.0 ],
                    "bgfillcolor_proportion": 0.5,
                    "bgfillcolor_type": "color",
                    "fontface": 1,
                    "gradient": 1,
                    "id": "obj-40",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 284.0, 516.0, 269.0, 22.0 ],
                    "text": "https://github.com/valeriorlandini/theinformer",
                    "textcolor": [ 0.0, 0.015686274509804, 0.396078431372549, 1.0 ]
                }
            },
            {
                "box": {
                    "hidden": 1,
                    "id": "obj-36",
                    "linecount": 2,
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 10.0, 525.0, 129.0, 36.0 ],
                    "text": ";\r\nmax launchbrowser $1"
                }
            },
            {
                "box": {
                    "bubble": 1,
                    "fontname": "Arial",
                    "fontsize": 13.0,
                    "id": "obj-19",
                    "linecount": 7,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 227.0, 113.26607578992844, 322.0, 115.0 ],
                    "text": "when dict is turned on, the messages are formatted to directly feed a dictionary, otherwise the external ouptuts messages in the format [descriptor] [value]\nwhen normalize is turned on, all the descriptors are scaled inside [0, 1] range: consider that for some descriptors this is done using an euristic approach, so it may not be suitable for scientific purposes"
                }
            },
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 33.0, 238.0, 252.0, 22.0 ],
                    "text": "informer.timedesc 8192 @dict 1 @overlap 0.5"
                }
            },
            {
                "box": {
                    "id": "obj-5",
                    "maxclass": "live.line",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 55.0, 805.0, 5.0 ]
                }
            },
            {
                "box": {
                    "background": 1,
                    "fontface": 0,
                    "fontname": "Arial",
                    "fontsize": 16.0,
                    "id": "obj-1",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 80.0, 805.0, 25.0 ],
                    "text": "Computation of a series of time domain descriptors in real time, with user definable buffer size and overlap.",
                    "textcolor": [ 0.956862745098039, 0.764705882352941, 0.450980392156863, 1.0 ]
                }
            },
            {
                "box": {
                    "background": 1,
                    "fontface": 0,
                    "fontname": "Arial",
                    "fontsize": 20.0,
                    "id": "obj-3",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 55.0, 805.0, 29.0 ],
                    "text": "time domain descriptors real time analysis"
                }
            },
            {
                "box": {
                    "background": 1,
                    "fontface": 2,
                    "fontname": "Arial",
                    "fontsize": 12.0,
                    "id": "obj-24",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 500.0, 805.0, 20.0 ],
                    "text": "handmade in italy by valerio orlandini",
                    "textjustification": 1
                }
            },
            {
                "box": {
                    "background": 1,
                    "fontface": 1,
                    "fontname": "Arial",
                    "fontsize": 36.0,
                    "id": "obj-21",
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 16.0, 12.0, 805.0, 48.0 ],
                    "text": "informer.timedesc"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "order": 1,
                    "source": [ "obj-10", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-26", 0 ],
                    "order": 0,
                    "source": [ "obj-10", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "source": [ "obj-11", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "source": [ "obj-12", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "source": [ "obj-14", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-16", 0 ],
                    "source": [ "obj-15", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-15", 0 ],
                    "hidden": 1,
                    "source": [ "obj-17", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-15", 0 ],
                    "source": [ "obj-2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-23", 0 ],
                    "hidden": 1,
                    "source": [ "obj-22", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 0 ],
                    "hidden": 1,
                    "source": [ "obj-23", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 4 ],
                    "source": [ "obj-25", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-13", 1 ],
                    "order": 0,
                    "source": [ "obj-26", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-13", 0 ],
                    "order": 1,
                    "source": [ "obj-26", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "source": [ "obj-4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-36", 0 ],
                    "hidden": 1,
                    "source": [ "obj-40", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 1 ],
                    "source": [ "obj-7", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 3 ],
                    "source": [ "obj-8", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 2 ],
                    "source": [ "obj-9", 0 ]
                }
            }
        ],
        "autosave": 0,
        "boxgroups": [
            {
                "boxes": [ "obj-27", "obj-22" ]
            }
        ]
    }
}