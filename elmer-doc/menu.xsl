<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  version="1.0">
  
  <xsl:output method="html"/>
  <xsl:template match="/">
    <html>
      <head>
        <title>
          Elmer: <xsl:value-of select="html/head/title"/>
        </title>
        <link rel="stylesheet" href="elmer.css"/>
        <!--        <link rel="stylesheet" href="http://www.csc.fi/css/csc.css" type="text/css"/> -->

      </head>

      <body bgcolor="#FFFFFF">
        <div class="all">
          <div id="top">
            <h1>Elmer</h1>
            <!-- tools -->
            <div style="text-align: right;"><a href="@print_version@"><img style="border: 0px;" border="0" src="images/print_icon.gif"/></a></div>
          </div>

          <div id="middle">
            <!--          <h1><xsl:value-of select="/html/head/title"/></h1> -->
            <xsl:copy-of select="/html/body"/>

            <!-- copyright -->
            <div id="copyrights">
              <hr/>
              <script language="JavaScript1.2">
                <![CDATA[ 
                  <!--
                       var time=new Date();
                       var year=time.getYear();
                       document.write("<span class=\"copyright\">Copyright CSC " + year + ", ");
                       document.write("last modified:" + document.lastModified + "</span>");
                    // -->
                  ]]> 
                </script>
            </div>

          </div>


          <div id="left">
            <!--
Elmer
Overview
ElmerFront
ElmerSolver
ElmerPost
ElmerGrid
Examples
Papers
Download
Tutorials
Registration
News
> Contributors
Contact information

          -->
            
            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'front'">
                <a class="catsel" href="front.html">Elmer</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class="cat" href="front.html">Elmer</a><br/>
              </xsl:otherwise>
            </xsl:choose>

            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'download'">
                <a class="catsel" href="download.html">Download</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class="cat" href="download.html">Download</a><br/>
              </xsl:otherwise>
            </xsl:choose>

            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'compiling'">
                <a class="catsel" href="compilation.html">Compiling</a><br/>
                <a class="subcat" href="automake-nano-howto.html">Autotools HOWTO</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class="cat" href="compilation.html">Compiling</a><br/>
              </xsl:otherwise>
            </xsl:choose>


            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'documentation'">
                <a class="catsel" href="manuals.html">Documentation</a><br/>
                <a class="subcat" href="http://www.csc.fi/elmer/tutorials/index.phtml">Tutorials</a><br/>
                <a class="subcat" href="http://www.csc.fi/elmer/papers/index.phtml">Papers</a><br/>
                <a class="subcat" href="http://www.csc.fi/elmer/examples/index.phtml">Examples</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a href="manuals.html">Documentation</a><br/>
              </xsl:otherwise>
            </xsl:choose>


            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'screenshots'">
                <a class="catsel" href="screenshots.html">Screenshots</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class ="cat" href="screenshots.html">Screenshots</a><br/>
              </xsl:otherwise>
            </xsl:choose>


            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'lists'">
                <a class="catsel" href="lists.html">Mailing lists</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class="cat" href="lists.html">Mailing lists</a><br/>
              </xsl:otherwise>
            </xsl:choose>


            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'contributors'">
                <a class="catsel" href="contributors.html">Contributors</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class="cat" href="contributors.html">Contributors</a><br/>
              </xsl:otherwise>
            </xsl:choose>


            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'faq'">
                <a class="catsel" href="faq.html">FAQ</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class="cat" href="faq.html">FAQ</a><br/>
              </xsl:otherwise>
            </xsl:choose>


            <xsl:choose>
              <xsl:when test="/html/head/meta/@category = 'contact'">
                <a class="catsel" href="contact.html">Contact info</a><br/>
              </xsl:when>
              <xsl:otherwise>
                <a class="cat" href="contact.html">Contact info</a><br/>
              </xsl:otherwise>
            </xsl:choose>

        </div>

        <!--        <div id="right">
          <h3>News:</h3>
          <p>News items?</p>
        </div> -->

      </div>
      </body>


    </html>
  </xsl:template>
</xsl:stylesheet>



