using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml;

namespace WinFormsApp
{

    /// <summary>
    /// Utility class to provide documentation for various types where available with the assembly
    /// </summary>
    public static class DocumentationExtensions
    {
        /// <summary>
        /// Provides the documentation comments for a specific method
        /// </summary>
        /// <param name="methodInfo">The MethodInfo (reflection data ) of the member to find documentation for</param>
        /// <returns>The XML fragment describing the method</returns>
        public static XmlElement GetDocumentation(this MethodInfo methodInfo)
        {
            // Calculate the parameter string as this is in the member name in the XML
            var parametersString = "";
            foreach (var parameterInfo in methodInfo.GetParameters())
            {
                if (parametersString.Length > 0)
                {
                    parametersString += ",";
                }

                parametersString += parameterInfo.ParameterType.FullName;
            }

            //AL: 15.04.2008 ==> BUG-FIX remove “()” if parametersString is empty
            if (parametersString.Length > 0)
                return XmlFromName(methodInfo.DeclaringType, 'M', methodInfo.Name + "(" + parametersString + ")");
            else
                return XmlFromName(methodInfo.DeclaringType, 'M', methodInfo.Name);
        }

        /// <summary>
        /// Provides the documentation comments for a specific member
        /// </summary>
        /// <param name="memberInfo">The MemberInfo (reflection data) or the member to find documentation for</param>
        /// <returns>The XML fragment describing the member</returns>
        public static XmlElement GetDocumentation(this MemberInfo memberInfo)
        {
            // First character [0] of member type is prefix character in the name in the XML
            return XmlFromName(memberInfo.DeclaringType, memberInfo.MemberType.ToString()[0], memberInfo.Name);
        }
        /// <summary>
        /// Returns the Xml documenation summary comment for this member
        /// </summary>
        /// <param name="memberInfo"></param>
        /// <returns></returns>
        public static string GetSummary(this MemberInfo memberInfo)
        {
            var element = memberInfo.GetDocumentation();
            var summaryElm = element?.SelectSingleNode("summary");
            if (summaryElm == null) return "";

            if (summaryElm.InnerText.Contains("\r\n"))
            {
                // remove spaces of newline header
                string tmp = summaryElm.InnerText.Trim();

                //var regex = new Regex("\\s+");  // Reges(string 정규식패턴)
                var regex = new Regex("\n\\s+");  // Reges(string 정규식패턴)
                string r = regex.Replace(tmp, "\n");
                return r;
            }

            return summaryElm.InnerText.Trim();
        }

        /// <summary>
        /// Provides the documentation comments for a specific type
        /// </summary>
        /// <param name="type">Type to find the documentation for</param>
        /// <returns>The XML fragment that describes the type</returns>
        public static XmlElement GetDocumentation(this Type type)
        {
            // Prefix in type names is T
            return XmlFromName(type, 'T', "");
        }

        /// <summary>
        /// Gets the summary portion of a type's documenation or returns an empty string if not available
        /// </summary>
        /// <param name="type"></param>
        /// <returns></returns>
        public static string GetSummary(this Type type)
        {
            var element = type.GetDocumentation();
            var summaryElm = element?.SelectSingleNode("summary");
            if (summaryElm == null) return "";
            return summaryElm.InnerText.Trim();
        }

        /// <summary>
        /// Obtains the XML Element that describes a reflection element by searching the 
        /// members for a member that has a name that describes the element.
        /// </summary>
        /// <param name="type">The type or parent type, used to fetch the assembly</param>
        /// <param name="prefix">The prefix as seen in the name attribute in the documentation XML</param>
        /// <param name="name">Where relevant, the full name qualifier for the element</param>
        /// <returns>The member that has a name that describes the specified reflection element</returns>
        private static XmlElement XmlFromName(this Type type, char prefix, string name)
        {
            string fullName;

            if (string.IsNullOrEmpty(name))
                fullName = prefix + ":" + type.FullName;
            else
                fullName = prefix + ":" + type.FullName + "." + name;

            var xmlDocument = XmlFromAssembly(type.Assembly);

            var matchedElement = xmlDocument["doc"]["members"].SelectSingleNode("member[@name='" + fullName + "']") as XmlElement;

            return matchedElement;
        }

        /// <summary>
        /// A cache used to remember Xml documentation for assemblies
        /// </summary>
        private static readonly Dictionary<Assembly, XmlDocument> Cache = new Dictionary<Assembly, XmlDocument>();

        /// <summary>
        /// A cache used to store failure exceptions for assembly lookups
        /// </summary>
        private static readonly Dictionary<Assembly, Exception> FailCache = new Dictionary<Assembly, Exception>();

        /// <summary>
        /// Obtains the documentation file for the specified assembly
        /// </summary>
        /// <param name="assembly">The assembly to find the XML document for</param>
        /// <returns>The XML document</returns>
        /// <remarks>This version uses a cache to preserve the assemblies, so that 
        /// the XML file is not loaded and parsed on every single lookup</remarks>
        public static XmlDocument XmlFromAssembly(this Assembly assembly)
        {
            if (FailCache.ContainsKey(assembly))
            {
                throw FailCache[assembly];
            }

            try
            {

                if (!Cache.ContainsKey(assembly))
                {
                    // load the docuemnt into the cache
                    Cache[assembly] = XmlFromAssemblyNonCached(assembly);
                }

                return Cache[assembly];
            }
            catch (Exception exception)
            {
                FailCache[assembly] = exception;
                throw;
            }
        }

        /// <summary>
        /// Loads and parses the documentation file for the specified assembly
        /// </summary>
        /// <param name="assembly">The assembly to find the XML document for</param>
        /// <returns>The XML document</returns>
        private static XmlDocument XmlFromAssemblyNonCached(Assembly assembly)
        {
            var assemblyFilename = assembly.Location;

            if (!string.IsNullOrEmpty(assemblyFilename))
            {
                StreamReader streamReader;

                try
                {
                    streamReader = new StreamReader(Path.ChangeExtension(assemblyFilename, ".xml"));
                }
                catch (FileNotFoundException exception)
                {
                    throw new Exception("XML documentation not present (make sure it is turned on in project properties when building)", exception);
                }

                var xmlDocument = new XmlDocument();
                xmlDocument.Load(streamReader);
                return xmlDocument;
            }
            else
            {
                throw new Exception("Could not ascertain assembly filename", null);
            }
        }
    }



    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Button1_Click(object sender, EventArgs e)
        {

            WinRTComponent.Class winRTClass = new WinRTComponent.Class();
            String textFromComponent = winRTClass.MyProperty.ToString();
            TextBox1.Text = textFromComponent;

            var popup = new Popup(textFromComponent);
            popup.Show(this);
        }

        private void TextBox1_TextChanged(object sender, EventArgs e)
        {

        }


        WASAPI.Class wasClass = null;
        private void button2_Click(object sender, EventArgs e)
        {
            if(wasClass == null)
                wasClass = new WASAPI.Class();
            string text = wasClass.MyProperty.ToString();

            wasClass.Init(0);


        }


        class FieldEntity
        {
            public FieldInfo info;
            public string Description;
            public override string ToString()
            {
                return info.Name;
            }
        }

      


        private void button3_Click(object sender, EventArgs e)
        {
            if (wasClass == null)
                wasClass = new WASAPI.Class();
            string text = wasClass.MyProperty.ToString();

            wasClass.Init(1);


            WASAPI.Env env = new WASAPI.Env();
            env.Prm1 = 2;
            //int env_prm2 = WASAPI.Env.Method1();
            int ret = env.Prm1;

            int nb_prms = WASAPI.Env.NumPrms;
            WASAPI.Env.NumPrms = 0;

            
            Type src = typeof(WASAPI.Env);
            if(src.IsCOMObject && src.AssemblyQualifiedName.Contains("WindowsRuntime"))
            {
                // xml file is generated from dll project.
                string dllname = src.Namespace; // "WASAPI";

                string type = "M"; // read type from dll assembly is P, but documented type is M
                string prefix = $"{type}:winrt.{dllname}.implementeation.{src.Name}";
                // TODO: SelectSingleNode("
                // F: field
                // M: method   (documented type)
                // P: property ()
            }
            var props = src.GetProperties();
            
            var fieldlist = props.Where(x => x.PropertyType == typeof(int));
            foreach (var field in fieldlist)
            {
                var fentity = new FieldEntity();

                if (field.GetGetMethod().IsStatic)
                {
                    var v = field.GetGetMethod().Invoke(null, null);
                }

                //fentity.Description = src.GetMember(field.Name)[0].GetSummary();
                var prop = src.GetProperty(field.Name);

                fentity.Description = prop.GetSummary();

            }

        }
    }
}
