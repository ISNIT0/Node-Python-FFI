{
  "targets": [
    {
      "target_name": "pythonffi",
      "sources": [ "index.cc" ],
      "include_dirs": [
        "/usr/local/Cellar/python3/3.6.3/Frameworks/Python.framework/Versions/3.6/include/python3.6m",
      ],
      "libraries": [
        "-L/usr/local/Cellar/python3/3.6.3/Frameworks/Python.framework/Versions/3.6/lib",
        "-lpython3.6"
      ]
    }
  ]
}