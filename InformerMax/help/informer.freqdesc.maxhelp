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
        "rect": [ 425.0, 330.0, 847.0, 549.0 ],
        "boxes": [
            {
                "box": {
                    "hidden": 1,
                    "id": "obj-17",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "bang" ],
                    "patching_rect": [ 300.0, 245.0, 58.0, 22.0 ],
                    "text": "loadbang"
                }
            },
            {
                "box": {
                    "id": "obj-4",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 279.0, 206.76607578992844, 183.0, 22.0 ],
                    "text": "pfft~ frequency_descriptors 8192"
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
                    "patching_rect": [ 512.0, 137.76607578992844, 81.28654617071152, 66.0 ],
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
                    "patching_rect": [ 593.0, 353.0, 123.0, 26.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-25",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "signal" ],
                    "patching_rect": [ 652.0, 217.76607578992844, 35.0, 22.0 ],
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
                    "patching_rect": [ 495.0, 216.76607578992844, 29.5, 22.0 ],
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
                    "patching_rect": [ 495.0, 137.76607578992844, 18.0, 66.0 ],
                    "size": 4,
                    "value": 0
                }
            },
            {
                "box": {
                    "id": "obj-16",
                    "maxclass": "dict.view",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 27.0, 138.26607578992844, 236.0, 281.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-15",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 5,
                    "outlettype": [ "dictionary", "", "", "", "" ],
                    "patching_rect": [ 279.0, 286.0, 61.0, 22.0 ],
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
                    "patching_rect": [ 593.0, 433.0, 45.0, 45.0 ]
                }
            },
            {
                "box": {
                    "id": "obj-10",
                    "maxclass": "newobj",
                    "numinlets": 5,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 593.0, 267.76607578992844, 78.0, 22.0 ],
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
                    "patching_rect": [ 623.0, 169.76607578992844, 66.0, 22.0 ],
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
                    "patching_rect": [ 637.0, 193.76607578992844, 44.0, 22.0 ],
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
                    "patching_rect": [ 608.0, 137.76607578992844, 150.0, 30.0 ],
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
                    "bubbleside": 2,
                    "fontname": "Arial",
                    "fontsize": 13.0,
                    "id": "obj-19",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 284.0, 137.76607578992844, 174.0, 55.0 ],
                    "text": "open the pfft~ patch to see how the external is used",
                    "textjustification": 1
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
                    "text": "Computation of a series of frequency domain descriptors in real time from the magnitude values of a FFT.",
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
                    "text": "frequency domain descriptors real time analysis"
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
                    "text": "informer.freqdesc"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-26", 0 ],
                    "midpoints": [ 602.5, 291.0, 602.5, 291.0 ],
                    "order": 0,
                    "source": [ "obj-10", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-4", 0 ],
                    "midpoints": [ 602.5, 291.0, 351.0, 291.0, 351.0, 240.0, 276.0, 240.0, 276.0, 201.0, 288.5, 201.0 ],
                    "order": 1,
                    "source": [ "obj-10", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-16", 0 ],
                    "midpoints": [ 288.5, 282.0, 276.0, 282.0, 276.0, 123.0, 36.5, 123.0 ],
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
                    "destination": [ "obj-23", 0 ],
                    "hidden": 1,
                    "midpoints": [ 504.5, 204.0, 504.5, 204.0 ],
                    "source": [ "obj-22", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 0 ],
                    "hidden": 1,
                    "midpoints": [ 504.5, 252.0, 602.5, 252.0 ],
                    "source": [ "obj-23", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 4 ],
                    "midpoints": [ 661.5, 240.0, 661.5, 240.0 ],
                    "source": [ "obj-25", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-13", 1 ],
                    "midpoints": [ 602.5, 420.0, 628.5, 420.0 ],
                    "order": 0,
                    "source": [ "obj-26", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-13", 0 ],
                    "midpoints": [ 602.5, 381.0, 602.5, 381.0 ],
                    "order": 1,
                    "source": [ "obj-26", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-15", 0 ],
                    "midpoints": [ 288.5, 231.0, 288.5, 231.0 ],
                    "source": [ "obj-4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-36", 0 ],
                    "hidden": 1,
                    "midpoints": [ 293.5, 540.0, 150.0, 540.0, 150.0, 522.0, 19.5, 522.0 ],
                    "source": [ "obj-40", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 1 ],
                    "midpoints": [ 617.5, 168.0, 617.25, 168.0 ],
                    "source": [ "obj-7", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 3 ],
                    "midpoints": [ 646.5, 216.0, 646.75, 216.0 ],
                    "source": [ "obj-8", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-10", 2 ],
                    "midpoints": [ 632.5, 192.0, 632.0, 192.0 ],
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