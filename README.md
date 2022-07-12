# DBconv
A tool to convert line oriented databases to separator-delimited format
dbconv(1)                  Database format converter                 dbconv(1)



NAME
       dbconv - convert a line-oriented database to separator-oriented format

SYNOPSIS
       dbconv  [-r ref_file] [-d database] [-o output] [-f c] [-e c] [-1] [-q]
       [-h]

DESCRIPTION
       dbconv is a tool to convert a database structured as

              FIELD1    1st field contents
              FIELD2    2nd field contents
              FIELD2    2nd field contents (continued)
               ... etc ...
              EOR
       i.e. with each field spanning one or more lines  and  preceded  by  the
       field  name  into  a  database where each record eats up only one line,
       with fields being separated by a special delimiter field.

              1st field contents\t2nd field contents 2nd field contents (continued)\t ... etc ...

       By default dbconv acts as a filter, reading the  database  file  to  be
       converted from its standard input and sending the converted data to its
       standard output.

       The format of the input database is specified by a reference file which
       states how to identify records and fields (see below).

       By  default, dbconv assumes that the output format to use is Tab Delim‐
       ited Format (i.e. fields are separated by TAB characters with TAB char‐
       acters not allowed within fields).
       Command  line  options  allow  you  to tweak the behaviour to suit your
       needs: you may change the output field and record separator characters,
       include  an initial description line with field metadata, or quote spe‐
       cial characters to allow them inside fields.

OPTIONS
       -c ref_file
              Specify configuration file containing the  reference  layout  of
              the  input  database (default DBconv.cf). See below for more de‐
              tails.

       -d database
              Database file to be converted (if unspecified, read  from  stan‐
              dard input)

       -o output
              Output file name (if not specified, use standard output)

       -f delimiter
              Character  to use as the field delimiter in the converted output
              file (defaults to using a tabulator). Only the  first  character
              of  the  argument following -f is considered. This option allows
              you to switch from a Tab delimited file to  other  formats  like
              Comma  Separated Values (CSV) by simply stating an alternate de‐
              limiter character (e.g. '-f ,' for CSV).

       -r delimiter
              Character to use as the record delimiter in the converted output
              file (default to using a line feed). Only the first character of
              the argument following -r is considered.

       -q     Escape special characters using double quotes.  Special  charac‐
              ters  are  the  output  field  and  record separators and double
              quotes (") themselves.  Normally  field  and  record  separators
              should not appear within the fields, but since we are converting
              between formats it is conceivable that they do.  In  this  case,
              and  if you select -q, when a field contains delimiter (field or
              record) characters, the entire field will be enclosed in  double
              quotes,  and  any double quotes that might appear within are es‐
              caped by doubling them. This allows you to conform with MS  pur‐
              ported (and often unsupported by themselves) standards.

       -1     Prepend  a first record containing the field names. This is use‐
              ful to carry the field name information over with the data  when
              porting  databases  so  you can tell which data corresponds with
              which field from this header record.

       -h     Print a short usage summary and terse help on available options.

CONFIGURATION FILE
       Input database file specifications are read from a configuration  file,
       named by default DBConv.cf unless the -c file option is used to specify
       another one.

       The configuration file describes how to  identify  records  and  fields
       within  the input database file (i.e. the structure of the file) assum‐
       ing it is a line oriented file where each field is  stored  in  one  or
       more  lines,  preceded by a field name tag in each of them. Records are
       separated by a special tag (which may be empty meaning an empty line).

       Leading white space in the configuration file is  ignored  and  may  be
       used freely to format it in order to make it more readable.

       The  file  and record identifiers are described each in a separate line
       starting with the tag string enclosed in double quotes (possibly  after
       some  white  space),  e.g.  "FIELD  NAME". If the tag contains a double
       quote, it can be included by preceding it with a \ (escape)  character,
       as  in  "Field \"2\" here". If the \ character itself must be included,
       it can be scaped by repeating it. Actually, \ may scape any  character.
       A tag may be and empty string (represented by "").

       Record separator
           The  first  specification in the configuration file must be that of
           the record separator. Following it (in separate, independent lines)
           go field specifications.

       Field specification
           Field  specifications  follow  after  the initial record separator,
           each in its own line. The specification starts with the field iden‐
           tification  tag  (possibly after some white space which will be ig‐
           nored) enclosed in double quotes as described above.  This  is  the
           string  of characters that identify this file contents in the input
           database file.
           The tag may be optionally followed by a number which is interpreted
           as  a  recommended  size  for  the field. This number is not a hard
           limit, rather it is orientative, and if a field  needs  more  space
           the  program will adapt itself by allocating more space on the fly.
           In fact, the only reason for it is to increase performance in  some
           cases, and you can normally do without it at all.
           Any other text following the tag and the optional number (if speci‐
           fied) will be ignored, which comes handy  for  adding  comments  to
           each field.

       Comments
           You  may  intersperse  blank lines and comments withing the file to
           document it. As we stated white spaces are ignored, and so are  any
           lines that do not begin (after any blanks) with a double quote (the
           hallmark of a tag for a record or field definition).


       Configuration file example
                First specification must be that of the record separator:
           " \"Record\\separator\" "
           Which would stand for the string [ "Record\separator" ]

                Field specifications may be followed by a recommended size:
            "Field1" 125 chars
            "Fld2: " or not
            " Fld3:" if not followed by a number, rest of line is ignored

                All of it may be interspersed by blank lines or comments
           (lines not beginning -after any blanks- with a double quote).

                If a quote is not closed then the rest of the line is taken
           as the field name:
                " Field #4: 125<EOL>
                is taken as [ Field #4: 125<EOL>] (<EOL> means here the end
           of line)

                As an example this file would be interpreted as
                [ "Record\separator" ]
                [Field1]
                [Fld2: ]
                [ Fld3:]
                [ Field #4: 125<EOL>]

EXAMPLES
       The following examples should give you a feeling of what can be done:

              cat database | dbconv > output

       this example uses dbconv as a filter with all default options: it  will
       read  the data (formatted according to an existing DBConv.cf file) from
       stdin and output the results in stdout.

              dbconv -c config -d database -o output

       This command would convert database described by file config to Tab-de‐
       limited format, storing the converted data in output.

       Same  as before, but in this case we generate a CSV file, with a header
       record containing the field names, and quoting special characters  when
       present (so it may be easily imported into e.g. Excel).

       A  much more elaborate example may be as follows: let's assume you want
       to generate a password database from an LDAP LDIF file. You might  dump
       the required fields using ldapsearch (1):

              ldapsearch -LLL -h ldap.example.com \
                  -D "cn=manager,dc=example,dc=com" \
                  -b "ou=People,dc=example,dc=com" \
                  uid userPassword uidNumber \
                  gidNumber gecos loginShell homeDirectory \
                  > database.ldif

       Then create a configuration file named DBConv.cf for dbconv like

              ""
              "uid: "
              "userPassword:: "
              "uidNumber: "
              "gidNumber: "
              "gecos: "
              "loginShell: "
              "homeDirectory: "

       i.e. records are separated by blank lines, and contain the above fields
       (no size hints are included).

       Now invoke dbconv stating that you want fields separated by a colon

              dbconv -d database.ldif -o passwd.file -f :

       and you should be (almost) done. This example  doesn't  actually  fully
       work  since  the password would have been retireved base64 encoded from
       the LDAP server (hence the :: in the userPassword field), but  you  get
       the idea. The output of the command would indeed look something like

              daemon:e2NyeXB0fXg=:11648:1:1:daemons,,,:/dev/null:/
              bin:e2NyeXB0fSo=:9724:2:2:System Tools Owner,,,:/dev/null:/bin
               ...

DIAGNOSTICS
       If all goes well, dbconv will exit with a 0 exit status.

       The  program  will report on its standard error if there is any problem
       opening the required files and exit with an status condition of 1.


       Other internal error situations during processing, like problems  read‐
       ing  or  writing  files,  or allocating working memory will result in a
       premature end with an error exit condition of 2. In this  case  partial
       convertion results may be available on the output file.

AUTHOR
       Written by Jose R. Valverde <jrvalverde@acm.org>

BUGS
       Surely many.

       Email  bug  reports to jrvalverde@acm.org.  Be sure to include the word
       “dbconv” somewhere in the “Subject:” field.

COPYRIGHT
       Copyright © 1991-2005 Jose R. Valverde
       This is free software; see the source for copying conditions.  There is
       NO  warranty;  not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR
       PURPOSE.


SEE ALSO
       tr (1), cut (1), sed (1), col (1), expand (1), unexpand (1),  cmp  (1),
       comm (1)




dbconv v3.0                        1991-2005                         dbconv(1)
