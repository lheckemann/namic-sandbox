

import java.util.logging.*;
import groovy.text.*;

class Field {
  String name = ''
  String type = ''
  Integer minOccurs = 1
  Integer maxOccurs = 1
  String toString() { return type; }
  boolean allowNull() { return minOccurs == 0; }
}
  

class SchemaClass {
  def derivedFrom = '';
  def useDerived = false;
  def isArray = false;
  def xmlTag = ''
  def name = '';
  // Name is key, type is value
  def simpleFields = new HashMap()
  // Name is key, type is value
  def arrayFields = new HashMap()
  def fields = []
  def attributes = []
  def getClassName() {
    if ( !isArray && useDerived ) { return derivedFrom; } else { return name; }
  }
  def isSimple ( t ) {
    switch ( t ) {
      case 'String': case 'Float': case 'Integer': case 'Boolean': case 'Date': return true;
      default: return false
    }
    return false;
  }
}




// Logger.getLogger("").addHandler ( new ConsoleHandler() )
// Logger.getLogger("XCEDE.SchemaToGORM").setLevel ( java.util.logging.Level.ALL )
def logger = Logger.getLogger ( 'XCEDE.SchemaToGORM' );


// Parse the command line
def cli = new CliBuilder ( usage: 'groovy SchemaToGORM <Schema.xsd> <OutputPath>' )
cli.h ( longOpt: 'help', 'usage information' )

def options = cli.parse ( args )

if ( options.arguments().size() != 2 || options.h ) { cli.usage(); System.exit ( 0 ); }

def SchemaFilename = options.arguments()[0]
def OutputPath = options.arguments()[1]
logger.info ( 'Found Schema: ' + SchemaFilename + ' and OutputPath: ' + OutputPath )

def SchemaFile = new File ( SchemaFilename )
def OutputDir = new File ( OutputPath )

if ( !SchemaFile.exists() ) { logger.severe ( "Filename: '" + SchemaFilename + "' does not exist." ); System.exit ( 1 ); }

if ( !OutputDir.exists() ) { OutputDir.mkdirs() }
if ( !OutputDir.exists() || !OutputDir.isDirectory() || !OutputDir.canWrite() ) { 
  logger.severe ( "Output path: '" + OutputPath + "' is not a directory, or is not writable." ); System.exit ( 1 ); 
}


// Load our XML
def path = new XmlSlurper().parse ( SchemaFile )

logger.info ( 'Parsed, found ' + path.complexType.size() + ' complex types' )

def cb = new ClassBuilder()
path.simpleType.each() { cb.processSimpleType ( it, cb.transformClassname ( it.@name ) ) }
path.complexType.each() { cb.processComplexType ( it ) }

System.out.println ( '\n\n =====  Starting to print ====' );
cb.printAllClasses ( OutputDir )

class ClassBuilder {
  def ClassTemplate = new File ( 'ClassTemplate.template' )
  def ClassList = [:]

  def printAllClasses ( OutputDir ) {
    /* First remap all simple classes */
    /*
    ClassList.each() { clentry ->
      def cl = clentry.value
      println ( 'PrintAllClasses: ' + cl.name )
      if ( cl.isSimple ( cl.derivedFrom ) ) {
        cl.useDerived = true
      }
    }
    */
      /*
      cl.arrayFields.each() { entry ->
        println ( "\t" + entry.key + ': ' + entry.value )
        // Does the class derive from a simple type?
        if ( !cl.isSimple ( entry.value ) ) {
          def DerivedClass = ClassList[entry.value]?.derivedFrom
          println ( "\tDerived class: ${DerivedClass}" )
          if ( cl.isSimple ( DerivedClass ) ) {
            println ( "\t\tChanging ${entry.value} to ${DerivedClass}" )
            entry.value = DerivedClass
          }
        } 
      } */ 

    // Now add the simple classes
    /*
    ClassList["String"] = new SchemaClass ( name: "String" )
    ClassList["Integer"] = new SchemaClass ( name: "Integer" )
    ClassList["Boolean"] = new SchemaClass ( name: "Boolean" )
    ClassList["Date"] = new SchemaClass ( name: "Date" )
    ClassList["Float"] = new SchemaClass ( name: "Float" )
    */  
    ClassList.each() { printClass ( OutputDir, it.value ) }
  }


  def printClass ( File OutputDir, cl ) {

    /* don't bother to write out simple classes */
    // if ( cl.isSimple ( cl.derivedFrom ) ) { return }

    /* Should write to a file... */
    def o = new FileWriter ( new File ( OutputDir, cl.name + '.groovy' ) )

    println ( ' ==== Writing class: ' + cl.name )
    def binding = [ "cl" : cl, "cb" : this ];
    def engine = new SimpleTemplateEngine()
    def template = engine.createTemplate ( ClassTemplate ).make ( binding )
    o.print ( template.toString() )
  }

  def transformClassname ( cn ) {
    switch ( cn.toString() ) {
      case 'xs:anyURI': return "String";
      case 'xs:string': return "String";
      case 'xs:float': return "Float";
      case 'xs:integer': 
      case 'xs:int': return "Integer";
      case 'xs:boolean': return "Boolean";
      case 'xs:dateTime': return "Date";
    }
    def ClassName = ''
    cn.toString().split ( '_' ).each { 
      if ( it.size() > 0 && it != 't' ) { 
        ClassName += it[0].toUpperCase() + it[1..-1]
      }
    }
    return ClassName
  }
  
  
  def processSimpleType ( t, ClassName ) {
    def c = new SchemaClass()
    c.name = ClassName
    c.xmlTag = t.@name - '_t'
    System.out.println ( 'ClassList: ' + ClassList.toString() )
    ClassList[ClassName] = c
    System.out.println ( 'SimpleType: ' + ClassName )
    /*  Are we an array? */
    if ( t.list.size() ) {
      /* Do nothing yet */
      System.out.println ( '\tList of: ' + transformClassname ( t.list.@itemType ) );
    } else {
      c.derivedFrom = transformClassname ( t.restriction.@base )
      System.out.println ( '\tDerived from: ' + transformClassname ( t.restriction.@base ) )
    }
  }


  def processComplexType ( t ) { return processComplexType ( t, null ); }
  def processComplexType ( t, ClassName ) {
    def logger = Logger.getLogger ( 'XCEDE.SchemaToGORM.processComplexType' );

    if ( !ClassName ) {
      ClassName = transformClassname ( t.@name.toString() )
    }
    def c = new SchemaClass()
    c.name = ClassName
    c.xmlTag = t.@type - '_t'
    ClassList[ClassName] = c
    System.out.println ( 'Class Name: ' + ClassName )
    if ( t.complexContent.extension.size() ) {
      System.out.println ( '\tDerived from: ' + transformClassname ( t.complexContent.extension.@base ) )
      c.derivedFrom = transformClassname ( t.complexContent.extension.@base )
    }
    
    if ( t.simpleContent.extension.size() ) {
      System.out.println ( '\tDerived from: ' + transformClassname ( t.simpleContent.extension.@base ) );
      c.derivedFrom = transformClassname ( t.simpleContent.extension.@base )
    }

    // See what we have inside
    t.children().each() {
      processContents ( it, c )
    }

  }

  def processContents ( t, c ) {
    System.out.println ( '\t ---- processContents -- name: ' + t.name() + ' contents: ' + t )
    switch ( t.name() ) {
      case 'complexContent' :
      case 'extension' :
      case 'sequence':        t.children().each() { processContents ( it, c ) }; break;
      case 'attribute':       processElement ( t, c, true ); break;
      case 'element':         processElement ( t, c, false ); break;
    }
    return
  }

  def processElement ( t, c, isAttribute ) {
    // May be a complex element, may be not
    def isArray = t.@maxOccurs != ''
    
    def type = 'String';
    if ( t.@type.size() ) { 
      // We have a type
      // System.out.println ( '\t' + transformClassname ( t.@type ) + ' ' + t.@name );
      type = transformClassname ( t.@type );
    }
    
    /* Do we need to define a new type? */
    if ( t.complexType.size() ) {
      type = transformClassname ( t.@name )
      processComplexType ( t.complexType, transformClassname ( t.@name ) )
    } else {
      if ( isArray ) {
        type = transformClassname ( t.@type )
        System.out.println ( '\tMaking simple class of type: ' + type );
        if ( c.isSimple ( type ) ) {
          type = transformClassname ( t.@name )
          def cc = new SchemaClass ()
          cc.name = type
          cc.xmlTag = t.@type - '_t'
          cc.useDerived = false
          cc.isArray = true
          cc.derivedFrom = transformClassname ( t.@type )
          ClassList[cc.name] = cc
        } else {
          def cc = new SchemaClass()
          System.out.println ( '\tMade simple class of ' + type )
          cc.name = type
          cc.xmlTag = t.@type - '_t'
          cc.useDerived = false
          cc.isArray = true
          ClassList[cc.name] = cc
        }
      }
    }
    def f = new Field()
    f.name = t.@name.toString()
    f.type = type
    
    f.minOccurs = Integer.valueOf ( t.@minOccurs.toString() ? t.@minOccurs.toString() : '1' )
    println ( '\t' + f.name + ' has ' + f.minOccurs + ' occurrances' )
    if ( f.minOccurs == 0 ) { println ( '\tFound optional ' + f.name ) }
    switch ( t.@maxOccurs.toString() ) {
      case 'unbounded': f.maxOccurs = 1000000; break;
      case '': f.maxOccurs = 1; break;
      default: f.maxOccurs = t.@maxOccurs.toString()
    }
    if ( isArray ) {
      c.arrayFields[t.@name.toString()] = f
      System.out.println ( '\thasMany[' + type + ':' + t.@name + ']' )
    } else {
      c.simpleFields[t.@name.toString()] = f
      System.out.println ( '\t' + type + ' ' + t.@name )
    }

    if ( isAttribute ) {
      c.attributes.push ( t.@name.toString() )
    } else {
      c.fields.push ( t.@name.toString() )
    }

    return
  }


}
