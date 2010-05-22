<?xml version="1.0" encoding="UTF-8"?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                              xmlns:dc="http://purl.org/dc/elements/1.1/">

<xsl:template match="/">
    <center>
        <p>
        <img>
          <xsl:attribute name="src">
            <xsl:text>http://localhost:8080/fedora/objects/</xsl:text>
            <xsl:value-of select="//dc:identifier"/>
            <xsl:text>/methods/demo:DualResolution/fullSize</xsl:text>
          </xsl:attribute>
        </img><br/>
        <xsl:value-of select="//dc:description"/>
        </p>
    </center>
</xsl:template>

</xsl:stylesheet>
