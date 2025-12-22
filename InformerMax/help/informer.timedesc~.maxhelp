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
        "rect": [ 708.0, 421.0, 847.0, 466.0 ],
        "boxes": [
            {
                "box": {
                    "bubble": 1,
                    "fontname": "Arial",
                    "fontsize": 13.0,
                    "id": "obj-20",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 284.0, 263.0, 169.0, 40.0 ],
                    "text": "arguments: descriptor to compute and buffer size"
                }
            },
            {
                "box": {
                    "fontface": 0,
                    "fontname": "Arial",
                    "fontsize": 12.0,
                    "id": "obj-18",
                    "maxclass": "number~",
                    "mode": 2,
                    "numinlets": 2,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "float" ],
                    "patching_rect": [ 33.0, 322.0, 77.0, 22.0 ],
                    "sig": 0.0
                }
            },
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
                    "patching_rect": [ 644.0, 306.0, 123.0, 26.0 ]
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
                    "id": "obj-13",
                    "maxclass": "ezdac~",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 644.0, 349.0, 45.0, 45.0 ]
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
                    "attr": "descriptor",
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
                    "patching_rect": [ 16.0, 417.0, 805.0, 5.0 ]
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
                    "patching_rect": [ 284.0, 440.0, 269.0, 22.0 ],
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
                    "patching_rect": [ 10.0, 449.0, 129.0, 36.0 ],
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
                    "patching_rect": [ 227.0, 113.26607578992844, 325.0, 115.0 ],
                    "text": "when dict is turned on, the messages are formatted to directly feed a dictionary, otherwise the external ouptuts messages in the format [descriptor] [value]\nwhen normalize is turned on, all the descriptors are scaled inside [0, 1] range: consider that for some descriptors this is done using an euristic approach, so it may not be suitable for scientific purposes"
                }
            },
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 33.0, 272.0, 238.0, 22.0 ],
                    "text": "informer.timedesc~ rms 8192 @overlap 0.5"
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
                    "patching_rect": [ 16.0, 424.0, 805.0, 20.0 ],
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
                    "text": "informer.timedesc~"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "midpoints": [ 653.5, 291.0, 510.0, 291.0, 510.0, 249.0, 42.5, 249.0 ],
                    "order": 1,
                    "source": [ "obj-10", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-26", 0 ],
                    "midpoints": [ 653.5, 291.0, 653.5, 291.0 ],
                    "order": 0,
                    "source": [ "obj-10", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "midpoints": [ 42.5, 168.0, 18.0, 168.0, 18.0, 258.0, 42.5, 258.0 ],
                    "source": [ "obj-11", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "midpoints": [ 42.5, 216.0, 42.5, 216.0 ],
                    "source": [ "obj-12", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "midpoints": [ 42.5, 144.0, 18.0, 144.0, 18.0, 258.0, 42.5, 258.0 ],
                    "source": [ "obj-14", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-18", 0 ],
                    "midpoints": [ 42.5, 297.0, 42.5, 297.0 ],
                    "source": [ "obj-2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-23", 0 ],
                    "hidden": 1,
                    "midpoints": [ 568.5, 204.0, 568.5, 204.0 ],
                    "source": [ "obj-22", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 0 ],
                    "hidden": 1,
                    "midpoints": [ 568.5, 252.0, 653.5, 252.0 ],
                    "source": [ "obj-23", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 4 ],
                    "midpoints": [ 712.5, 240.0, 712.5, 240.0 ],
                    "source": [ "obj-25", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-13", 1 ],
                    "midpoints": [ 653.5, 345.0, 679.5, 345.0 ],
                    "order": 0,
                    "source": [ "obj-26", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-13", 0 ],
                    "midpoints": [ 653.5, 333.0, 653.5, 333.0 ],
                    "order": 1,
                    "source": [ "obj-26", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-2", 0 ],
                    "midpoints": [ 42.5, 192.0, 30.0, 192.0, 30.0, 258.0, 42.5, 258.0 ],
                    "source": [ "obj-4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-36", 0 ],
                    "hidden": 1,
                    "midpoints": [ 293.5, 464.0, 150.0, 464.0, 150.0, 446.0, 19.5, 446.0 ],
                    "source": [ "obj-40", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 1 ],
                    "midpoints": [ 668.5, 168.0, 668.25, 168.0 ],
                    "source": [ "obj-7", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 3 ],
                    "midpoints": [ 697.5, 216.0, 697.75, 216.0 ],
                    "source": [ "obj-8", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 2 ],
                    "midpoints": [ 683.5, 192.0, 683.0, 192.0 ],
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