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
        "rect": [ 287.0, 422.0, 704.0, 302.0 ],
        "boxes": [
            {
                "box": {
                    "bubble": 1,
                    "fontname": "Arial",
                    "fontsize": 13.0,
                    "id": "obj-7",
                    "linecount": 11,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 416.0, 34.0, 240.0, 175.0 ],
                    "presentation_linecount": 7,
                    "text": "when dict is turned on, the messages are formatted to directly feed a dictionary, otherwise the external ouptuts messages in the format [descriptor] [value]\nwhen normalize is turned on, all the descriptors are scaled inside [0, 1] range: consider that for some descriptors this is done using an euristic approach, so it may not be suitable for scientific purposes"
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
                    "patching_rect": [ 261.0, 123.0, 150.0, 22.0 ]
                }
            },
            {
                "box": {
                    "bubble": 1,
                    "fontname": "Arial",
                    "fontsize": 13.0,
                    "id": "obj-16",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 213.0, 34.0, 198.0, 40.0 ],
                    "text": "use a fftinfo~ to correctly set the spectral frame size"
                }
            },
            {
                "box": {
                    "attr": "dict",
                    "id": "obj-14",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 261.0, 99.0, 150.0, 22.0 ]
                }
            },
            {
                "box": {
                    "bubble": 1,
                    "fontname": "Arial",
                    "fontsize": 13.0,
                    "id": "obj-19",
                    "linecount": 2,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 241.0, 169.0, 169.5, 40.0 ],
                    "text": "inlet 1: FFT magnitude\ninlet 2: FFT bin index"
                }
            },
            {
                "box": {
                    "id": "obj-9",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 154.0, 75.0, 84.0, 22.0 ],
                    "text": "frame_size $1"
                }
            },
            {
                "box": {
                    "id": "obj-6",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 4,
                    "outlettype": [ "int", "int", "int", "int" ],
                    "patching_rect": [ 143.0, 43.0, 50.5, 22.0 ],
                    "text": "fftinfo~"
                }
            },
            {
                "box": {
                    "id": "obj-5",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 52.0, 228.0, 188.0, 22.0 ],
                    "saved_object_attributes": {
                        "attr_comment": "Descriptors",
                        "c": "Descriptors"
                    },
                    "text": "out 1 @attr_comment Descriptors"
                }
            },
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "signal" ],
                    "patching_rect": [ 52.0, 93.0, 58.0, 22.0 ],
                    "text": "cartopol~"
                }
            },
            {
                "box": {
                    "id": "obj-2",
                    "maxclass": "newobj",
                    "numinlets": 1,
                    "numoutlets": 3,
                    "outlettype": [ "signal", "signal", "signal" ],
                    "patching_rect": [ 52.0, 43.0, 45.0, 22.0 ],
                    "text": "fftin~ 1"
                }
            },
            {
                "box": {
                    "id": "obj-1",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 52.0, 178.0, 175.0, 22.0 ],
                    "text": "informer.freqdesc 4096 @dict 1"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-5", 0 ],
                    "midpoints": [ 61.5, 201.0, 61.5, 201.0 ],
                    "source": [ "obj-1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 270.5, 123.0, 120.0, 123.0, 120.0, 165.0, 61.5, 165.0 ],
                    "source": [ "obj-14", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 1 ],
                    "midpoints": [ 87.5, 78.0, 141.0, 78.0, 141.0, 165.0, 217.5, 165.0 ],
                    "source": [ "obj-2", 2 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 1 ],
                    "midpoints": [ 74.5, 78.0, 100.5, 78.0 ],
                    "source": [ "obj-2", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "midpoints": [ 61.5, 66.0, 61.5, 66.0 ],
                    "source": [ "obj-2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 61.5, 117.0, 61.5, 117.0 ],
                    "source": [ "obj-3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 270.5, 156.0, 61.5, 156.0 ],
                    "source": [ "obj-4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-9", 0 ],
                    "midpoints": [ 163.0, 66.0, 163.5, 66.0 ],
                    "source": [ "obj-6", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 163.5, 165.0, 61.5, 165.0 ],
                    "source": [ "obj-9", 0 ]
                }
            }
        ],
        "autosave": 0
    }
}