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
        "rect": [ 710.0, 548.0, 704.0, 302.0 ],
        "boxes": [
            {
                "box": {
                    "attr": "normalize",
                    "id": "obj-8",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 245.0, 127.0, 173.0, 22.0 ]
                }
            },
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
                    "patching_rect": [ 431.0, 27.0, 244.0, 175.0 ],
                    "text": "when dict is turned on, the messages are formatted to directly feed a dictionary, otherwise the external ouptuts messages in the format [descriptor] [value]\nwhen normalize is turned on, all the descriptors are scaled inside [0, 1] range: consider that for some descriptors this is done using an euristic approach, so it may not be suitable for scientific purposes"
                }
            },
            {
                "box": {
                    "attr": "dict",
                    "id": "obj-4",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 245.0, 103.0, 173.0, 22.0 ]
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
                    "patching_rect": [ 202.0, 25.0, 198.0, 40.0 ],
                    "text": "use a fftinfo~ to correctly set the spectral frame size"
                }
            },
            {
                "box": {
                    "attr": "descriptor",
                    "id": "obj-14",
                    "maxclass": "attrui",
                    "numinlets": 1,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "parameter_enable": 0,
                    "patching_rect": [ 245.0, 79.0, 173.0, 22.0 ]
                }
            },
            {
                "box": {
                    "bubble": 1,
                    "fontname": "Arial",
                    "fontsize": 13.0,
                    "id": "obj-19",
                    "linecount": 5,
                    "maxclass": "comment",
                    "numinlets": 1,
                    "numoutlets": 0,
                    "patching_rect": [ 257.0, 161.0, 169.5, 85.0 ],
                    "text": "inlet 1: FFT magnitude\ninlet 2: FFT bin index\narguments: descriptor to compute and spectral frame size"
                }
            },
            {
                "box": {
                    "id": "obj-9",
                    "maxclass": "message",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "" ],
                    "patching_rect": [ 143.0, 66.0, 84.0, 22.0 ],
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
                    "patching_rect": [ 132.0, 34.0, 50.5, 22.0 ],
                    "text": "fftinfo~"
                }
            },
            {
                "box": {
                    "id": "obj-5",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 0,
                    "patching_rect": [ 41.0, 253.0, 101.0, 22.0 ],
                    "text": "fftout~ 1 @nofft 1"
                }
            },
            {
                "box": {
                    "id": "obj-3",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 2,
                    "outlettype": [ "signal", "signal" ],
                    "patching_rect": [ 41.0, 84.0, 58.0, 22.0 ],
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
                    "patching_rect": [ 41.0, 34.0, 45.0, 22.0 ],
                    "text": "fftin~ 1"
                }
            },
            {
                "box": {
                    "id": "obj-1",
                    "maxclass": "newobj",
                    "numinlets": 2,
                    "numoutlets": 1,
                    "outlettype": [ "signal" ],
                    "patching_rect": [ 41.0, 192.0, 212.0, 22.0 ],
                    "text": "informer.freqdesc~ peak 4096 @dict 1"
                }
            }
        ],
        "lines": [
            {
                "patchline": {
                    "destination": [ "obj-5", 0 ],
                    "midpoints": [ 50.5, 216.0, 50.5, 216.0 ],
                    "source": [ "obj-1", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 254.5, 102.0, 111.0, 102.0, 111.0, 177.0, 50.5, 177.0 ],
                    "source": [ "obj-14", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 1 ],
                    "midpoints": [ 76.5, 69.0, 129.0, 69.0, 129.0, 177.0, 243.5, 177.0 ],
                    "source": [ "obj-2", 2 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 1 ],
                    "midpoints": [ 63.5, 69.0, 89.5, 69.0 ],
                    "source": [ "obj-2", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-3", 0 ],
                    "midpoints": [ 50.5, 57.0, 50.5, 57.0 ],
                    "source": [ "obj-2", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 50.5, 108.0, 50.5, 108.0 ],
                    "source": [ "obj-3", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 254.5, 126.0, 50.5, 126.0 ],
                    "source": [ "obj-4", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-9", 0 ],
                    "midpoints": [ 152.0, 57.0, 152.5, 57.0 ],
                    "source": [ "obj-6", 1 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 254.5, 150.0, 50.5, 150.0 ],
                    "source": [ "obj-8", 0 ]
                }
            },
            {
                "patchline": {
                    "destination": [ "obj-1", 0 ],
                    "midpoints": [ 152.5, 177.0, 50.5, 177.0 ],
                    "source": [ "obj-9", 0 ]
                }
            }
        ]
    }
}