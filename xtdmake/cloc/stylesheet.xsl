<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="/">
        <html>
            <head>
                <title>Clock report</title>
                <!-- Latest compiled and minified CSS -->
                <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css" integrity="sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u" crossorigin="anonymous"/>
                <!-- Optional theme -->
                <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css" integrity="sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp" crossorigin="anonymous"/>
                <!-- Latest compiled and minified JavaScript -->
                <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js" integrity="sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa" crossorigin="anonymous"></script>
            </head>
            <body bgcolor="#ffffff" style="padding-top:50px;">
                <div class="container-fluid">
                    <div class="row">
                        <div class="col-lg-6 col-lg-offset-3">
                            <div class="panel panel-default">
                                <div class="panel-heading">Summary</div>
                                <div class="panel-body">
                                    <table class="table table-striped table-bordered table-hover table-condensed">
                                        <thead>
                                            <tr>
                                                <th>Language</th>
                                                <th>File count</th>
                                                <th>Blank lines</th>
                                                <th>Comments</th>
                                                <th>Code</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <xsl:apply-templates select="./results/languages"/>
                                        </tbody>
                                    </table>
                                </div>
                            </div>
                            <div class="panel panel-default">
                                <div class="panel-heading">File details</div>
                                <div class="panel-body">
                                    <table class="table table-striped table-bordered table-hover table-condensed">
                                        <thead>
                                            <tr>
                                                <th>File</th>
                                                <th>Language</th>
                                                <th>Blank lines</th>
                                                <th>Comments</th>
                                                <th>Code</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <xsl:apply-templates select="./results/files"/>
                                        </tbody>
                                    </table>
                                </div>
                            </div>
                        </div>
                    </div>
                </div>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="language">
        <tr>
            <td> <xsl:value-of select="./@name"/> </td>
            <td> <xsl:value-of select="./@files_count"/> </td>
            <td> <xsl:value-of select="./@blank"/> </td>
            <td> <xsl:value-of select="./@comment"/> </td>
            <td> <xsl:value-of select="./@code"/> </td>
        </tr>
    </xsl:template>

    <xsl:template match="file">
        <tr>
            <td> <xsl:value-of select="./@name"/> </td>
            <td> <xsl:value-of select="./@language"/> </td>
            <td> <xsl:value-of select="./@blank"/> </td>
            <td> <xsl:value-of select="./@comment"/> </td>
            <td> <xsl:value-of select="./@code"/> </td>
        </tr>
    </xsl:template>

    <xsl:template match="total">
        <tr>
            <td></td>
            <td> <xsl:value-of select="./@sum_files"/> </td>
            <td> <xsl:value-of select="./@blank"/> </td>
            <td> <xsl:value-of select="./@comment"/> </td>
            <td> <xsl:value-of select="./@code"/> </td>
        </tr>
    </xsl:template>

</xsl:stylesheet>
