#define XMLHelp "" \
"<?xml version=\"1.0\" encoding=\"utf-8\"?>\n" \
"<executable>\n" \
"  <category>registration</category>\n" \
"  <title>NAMIC sample registration</title>\n" \
"  <description>Registers two images together using a rigid transform and MI</description>\n" \
"  <version>1.0</version>\n" \
"  <documentationurl></documentationurl>\n" \
"  <license></license>\n" \
"  <contributor>Daniel Blezek</contributor>\n" \
"\n" \
"  <parameters>\n" \
"    <label>Registration Parameters</label>\n" \
"    <description>Parameters used for registration</description>\n" \
"    <integer>\n" \
"      <flag>b</flag>\n" \
"      <longflag>histogrambins</longflag>\n" \
"      <description>Number of histogram bins to use for Mattes Mutual Information</description>\n" \
"      <label>Histogram Bins</label>\n" \
"      <default>30</default>\n" \
"      <constraints>\n" \
"        <minimum>1</minimum>\n" \
"        <maximum>500</maximum>\n" \
"        <step>5</step>\n" \
"      </constraints>\n" \
"    </integer>\n" \
"\n" \
"    <integer>\n" \
"      <flag>s</flag>\n" \
"      <longflag>spatialsamples</longflag>\n" \
"      <description>Number of spatial samples to use in estimating Mattes Mutual Information</description>\n" \
"      <label>Spatial Samples</label>\n" \
"      <default>10000</default>\n" \
"      <constraints>\n" \
"        <minimum>1000</minimum>\n" \
"        <maximum>50000</maximum>\n" \
"        <step>1000</step>\n" \
"      </constraints>\n" \
"    </integer>\n" \
"\n" \
"    <string>\n" \
"      <flag>i</flag>\n" \
"      <longflag>iterations</longflag>\n" \
"      <description>Comma separated list of iterations must have the same number of elements as learning rate</description>\n" \
"      <label>Iterations</label>\n" \
"      <default>200,100</default>\n" \
"    </string>\n" \
"\n" \
"    <string>\n" \
"      <flag>l</flag>\n" \
"      <longflag>learningrate</longflag>\n" \
"      <description>Comma separated list of learning rates must have the same number of elements as iterations</description>\n" \
"      <label>Learning Rates</label>\n" \
"      <default>0.05,0.005</default>\n" \
"    </string>\n" \
"    \n" \
"    <double>\n" \
"      <longflag>translationscale</longflag>\n" \
"      <flag>t</flag>\n" \
"      <description>Relative scale of translations to rotations, i.e. a value of 100 means 10mm = 1 degree</description>\n" \
"      <label>Translation scaling</label>\n" \
"      <default>100.0</default>\n" \
"      <constraints>\n" \
"        <minimum>10.0</minimum>\n" \
"        <maximum>500.0</maximum>\n" \
"        <step>50.0</step>\n" \
"      </constraints>\n" \
"    </double>\n" \
"  </parameters>\n" \
"\n" \
"  <parameters>\n" \
"    <label>IO</label>\n" \
"    <description>Input/output parameters</description>\n" \
"    <image>\n" \
"      <name>Fixed</name>\n" \
"      <label>Fixed Image</label>\n" \
"      <channel>input</channel>\n" \
"      <index>0</index>\n" \
"      <description>Fixed image to register to</description>\n" \
"    </image>\n" \
"    <image>\n" \
"      <name>Moving</name>\n" \
"      <label>Moving Image</label>\n" \
"      <channel>input</channel>\n" \
"      <index>1</index>\n" \
"      <description>Moving image</description>\n" \
"    </image>\n" \
"    <image>\n" \
"      <name>Output</name>\n" \
"      <label>Output Volume</label>\n" \
"      <channel>output</channel>\n" \
"      <index>2</index>\n" \
"      <description>Resampled Moving Image</description>\n" \
"    </image>\n" \
"  </parameters>\n" \
"\n" \
"</executable>\n"
