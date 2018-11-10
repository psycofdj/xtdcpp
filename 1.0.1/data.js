var g_data = [{"childs": [{"name": "doc", "file": "tests/doc/html/index.html"}, {"status": "failure", "name": "doc-coverage", "label": "0 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "documented lines", "data": "%(documented)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(documented)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : doc-coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "tests/doc-coverage/index.html", "data": {"documented": 0, "total": 66}}, {"status": "failure", "name": "cloc", "label": "2 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "comment lines", "data": "%(comment)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "code lines", "data": "%(code)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% comment lines", "data": "int(float(%(comment)d) / (float(%(comment)d) + float(%(code)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : cloc", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "tests/cloc/cloc.html", "data": {"comment": 13, "code": 554}}, {"status": "success", "name": "cppcheck", "label": "0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0.5)", "label": "cppcheck error count", "data": "%(total)d"}]}, "options": {"title": {"text": "%(module)s : cppcheck", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "tests/cppcheck/cppcheck.html", "data": {"total": 0}}, {"status": "warning", "name": "check", "label": "0 / 0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "success tests", "data": "%(success)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "failure tests", "data": "%(failures)d"}]}, "options": {"title": {"text": "%(module)s : unittests", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "tests/check/index.html", "data": {"failures": 0, "success": 0}}, {"status": "failure", "name": "coverage", "label": "0 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "covered lines", "data": "%(covered)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(covered)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "tests/coverage/index.html", "data": {"covered": 0, "total": 132, "percent": "int((float(%(covered)d) / float(%(total)d)) * 100)"}}], "name": "tests"}, {"childs": [{"name": "doc", "file": "servers/doc/html/index.html"}, {"status": "failure", "name": "doc-coverage", "label": "22 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "documented lines", "data": "%(documented)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(documented)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : doc-coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "servers/doc-coverage/index.html", "data": {"documented": 55, "total": 241}}, {"status": "failure", "name": "cloc", "label": "18 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "comment lines", "data": "%(comment)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "code lines", "data": "%(code)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% comment lines", "data": "int(float(%(comment)d) / (float(%(comment)d) + float(%(code)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : cloc", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "servers/cloc/cloc.html", "data": {"comment": 426, "code": 1852}}, {"status": "success", "name": "cppcheck", "label": "0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0.5)", "label": "cppcheck error count", "data": "%(total)d"}]}, "options": {"title": {"text": "%(module)s : cppcheck", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "servers/cppcheck/cppcheck.html", "data": {"total": 0}}, {"status": "warning", "name": "check", "label": "0 / 0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "success tests", "data": "%(success)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "failure tests", "data": "%(failures)d"}]}, "options": {"title": {"text": "%(module)s : unittests", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "servers/check/index.html", "data": {"failures": 0, "success": 0}}, {"status": "failure", "name": "coverage", "label": "0 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "covered lines", "data": "%(covered)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(covered)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "servers/coverage/index.html", "data": {"covered": 0, "total": 1008, "percent": "int((float(%(covered)d) / float(%(total)d)) * 100)"}}], "name": "servers"}, {"childs": [{"name": "doc", "file": "network/doc/html/index.html"}, {"status": "failure", "name": "doc-coverage", "label": "17 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "documented lines", "data": "%(documented)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(documented)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : doc-coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "network/doc-coverage/index.html", "data": {"documented": 83, "total": 464}}, {"status": "failure", "name": "cloc", "label": "22 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "comment lines", "data": "%(comment)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "code lines", "data": "%(code)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% comment lines", "data": "int(float(%(comment)d) / (float(%(comment)d) + float(%(code)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : cloc", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "network/cloc/cloc.html", "data": {"comment": 936, "code": 3225}}, {"status": "success", "name": "cppcheck", "label": "0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0.5)", "label": "cppcheck error count", "data": "%(total)d"}]}, "options": {"title": {"text": "%(module)s : cppcheck", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "network/cppcheck/cppcheck.html", "data": {"total": 0}}, {"status": "warning", "name": "check", "label": "0 / 0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "success tests", "data": "%(success)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "failure tests", "data": "%(failures)d"}]}, "options": {"title": {"text": "%(module)s : unittests", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "network/check/index.html", "data": {"failures": 0, "success": 0}}, {"status": "failure", "name": "coverage", "label": "0 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "covered lines", "data": "%(covered)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(covered)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "network/coverage/index.html", "data": {"covered": 0, "total": 1967, "percent": "int((float(%(covered)d) / float(%(total)d)) * 100)"}}], "name": "network"}, {"childs": [{"name": "doc", "file": "core/doc/html/index.html"}, {"status": "success", "name": "doc-coverage", "label": "96 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "documented lines", "data": "%(documented)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(documented)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : doc-coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "core/doc-coverage/index.html", "data": {"documented": 335, "total": 347}}, {"status": "success", "name": "cloc", "label": "41 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "comment lines", "data": "%(comment)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "code lines", "data": "%(code)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% comment lines", "data": "int(float(%(comment)d) / (float(%(comment)d) + float(%(code)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : cloc", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "core/cloc/cloc.html", "data": {"comment": 2283, "code": 3266}}, {"status": "success", "name": "cppcheck", "label": "0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0.5)", "label": "cppcheck error count", "data": "%(total)d"}]}, "options": {"title": {"text": "%(module)s : cppcheck", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "core/cppcheck/cppcheck.html", "data": {"total": 0}}, {"status": "success", "name": "check", "label": "14 / 14", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "success tests", "data": "%(success)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "failure tests", "data": "%(failures)d"}]}, "options": {"title": {"text": "%(module)s : unittests", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "core/check/index.html", "data": {"failures": 0, "success": 14}}, {"status": "success", "name": "coverage", "label": "93 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "covered lines", "data": "%(covered)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(covered)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "core/coverage/index.html", "data": {"covered": 1403, "total": 1494, "percent": "int((float(%(covered)d) / float(%(total)d)) * 100)"}}], "name": "core"}, {"childs": [{"name": "doc", "file": "counters/doc/html/index.html"}, {"status": "failure", "name": "doc-coverage", "label": "1 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "documented lines", "data": "%(documented)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(documented)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : doc-coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "counters/doc-coverage/index.html", "data": {"documented": 2, "total": 157}}, {"status": "failure", "name": "cloc", "label": "2 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "comment lines", "data": "%(comment)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "code lines", "data": "%(code)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% comment lines", "data": "int(float(%(comment)d) / (float(%(comment)d) + float(%(code)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : cloc", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "counters/cloc/cloc.html", "data": {"comment": 29, "code": 1099}}, {"status": "success", "name": "cppcheck", "label": "0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0.5)", "label": "cppcheck error count", "data": "%(total)d"}]}, "options": {"title": {"text": "%(module)s : cppcheck", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "counters/cppcheck/cppcheck.html", "data": {"total": 0}}, {"status": "warning", "name": "check", "label": "0 / 0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "success tests", "data": "%(success)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "failure tests", "data": "%(failures)d"}]}, "options": {"title": {"text": "%(module)s : unittests", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "counters/check/index.html", "data": {"failures": 0, "success": 0}}, {"status": "failure", "name": "coverage", "label": "0 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "covered lines", "data": "%(covered)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(covered)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "counters/coverage/index.html", "data": {"covered": 0, "total": 436, "percent": "int((float(%(covered)d) / float(%(total)d)) * 100)"}}], "name": "counters"}, {"childs": [{"name": "doc", "file": "serializer/doc/html/index.html"}, {"status": "failure", "name": "doc-coverage", "label": "0 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "documented lines", "data": "%(documented)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(documented)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : doc-coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "serializer/doc-coverage/index.html", "data": {"documented": 0, "total": 57}}, {"status": "failure", "name": "cloc", "label": "6 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "comment lines", "data": "%(comment)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "code lines", "data": "%(code)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% comment lines", "data": "int(float(%(comment)d) / (float(%(comment)d) + float(%(code)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : cloc", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "serializer/cloc/cloc.html", "data": {"comment": 30, "code": 469}}, {"status": "success", "name": "cppcheck", "label": "0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0.5)", "label": "cppcheck error count", "data": "%(total)d"}]}, "options": {"title": {"text": "%(module)s : cppcheck", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "serializer/cppcheck/cppcheck.html", "data": {"total": 0}}, {"status": "warning", "name": "check", "label": "0 / 0", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "success tests", "data": "%(success)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "failure tests", "data": "%(failures)d"}]}, "options": {"title": {"text": "%(module)s : unittests", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}]}}}], "file": "serializer/check/index.html", "data": {"failures": 0, "success": 0}}, {"status": "failure", "name": "coverage", "label": "0 %", "graphs": [{"type": "line", "data": {"labels": [], "datasets": [{"borderColor": "rgba(51, 204, 51, 0.5)", "pointBorderColor": "rgba(31, 122, 31, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(31, 122, 31, 1)", "backgroundColor": "rgba(51, 204, 51, 0)", "label": "covered lines", "data": "%(covered)d"}, {"borderColor": "rgba(179, 0, 0, 0.5)", "pointBorderColor": "rgba(102, 0, 0, 1)", "yAxisID": "absolute", "pointBackgroundColor": "rgba(102, 0, 0, 1)", "backgroundColor": "rgba(179, 0, 0, 0)", "label": "total lines", "data": "%(total)d"}, {"borderColor": "rgba(102, 153, 255, 0.5)", "pointBorderColor": "rgba(0, 60, 179, 1)", "yAxisID": "percent", "pointBackgroundColor": "rgba(0, 60, 179, 1)", "backgroundColor": "rgba(102, 153, 255, 0)", "label": "% covered lines", "data": "int((float(%(covered)d) / float(%(total)d)) * 100)"}]}, "options": {"title": {"text": "%(module)s : coverage", "display": true}, "scales": {"xAxes": [{"ticks": {"fontSize": 12, "minRotation": 80}}], "yAxes": [{"position": "left", "ticks": {"fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "absolute", "display": true}, {"position": "right", "ticks": {"max": 100, "fontSize": 24, "beginAtZero": true}, "type": "linear", "id": "percent"}]}}}], "file": "serializer/coverage/index.html", "data": {"covered": 0, "total": 6, "percent": "int((float(%(covered)d) / float(%(total)d)) * 100)"}}], "name": "serializer"}]