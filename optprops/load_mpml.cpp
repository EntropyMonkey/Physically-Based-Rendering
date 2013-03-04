// 02576 Rendering Framework
// Written by Jeppe Revall Frisvad, 2010
// Copyright (c) DTU Informatics 2010

#include <map>
#include <vector>
#include <string>
#include <complex>
#include <algorithm>
#include "Util/XmlParser.h"
#include "Util/Parse.h"
#include "Medium.h"
#include "Interface.h"
#include "load_mpml.h"

using namespace std;
using namespace Util;

namespace
{
  map<string, Medium>* media = 0;
  map<string, Interface>* interfaces = 0;
  Medium* current = 0;
  string name;

  ColorType parseColorAttrib(const string& s)
  {
    if(s == "" || s == "rgb")
      return rgb;
    else if(s == "mono")
      return mono;
    else if(s == "xyz")
      return xyz;
    else if(s == "spectrum")
      return spectrum;

    cerr << "Warning loading " << name << ": Unknown color attribute" << endl;
    return rgb;
  }

  ScatteringUnit parseScatteringUnitAttrib(const string& s)
  {
    if(s == "" || s == "1/m")
      return per_meter;
    else if(s == "1/cm")
      return per_cm;
    else if(s == "1/mm")
      return per_mm;

    cerr << "Warning loading " << name << ": Unknown scattering unit attribute" << endl;
    return per_meter;
  }

  SpectralUnit parseSpectralUnitAttrib(const string& s)
  {
    if(s == "" || s == "nm")
      return nanometers;
    else if(s == "km")
      return kilometers;
    else if(s == "m")
      return meters;
    else if(s == "mm")
      return millimeters;
    else if(s == "um")
      return micrometers;
    else if(s == "Å")
      return angstrom;
    else if(s == "eV")
      return electron_volt;

    cerr << "Warning loading " << name << ": Unknown spectral unit attribute" << endl;    
    return nanometers;
  }

  template<class T> void checkColorSize(ColorType type, Color<T>& c)
  {
    switch(type)
    {
    case mono:
      if(c.size() != 1)
        cerr << "Warning loading " << name << ": Incorrect number of data values" << endl;
      break;
    case rgb:
    case xyz:
      if(c.size() == 1)
      {
        T tmp = c[0];
        c.resize(3);
        for(int i = 0; i < 3; ++i)
          c[i] = tmp;
      }
      if(c.size() != 3)
        cerr << "Warning loading " << name << ": Incorrect number of data values" << endl;
      break;
    }
  }

  void parseColorData(XmlElement& elem, ColorType type, Color<double>& c)
  {
    vector<double> data;
    parse(elem.body->text.c_str(), data);
    c.resize(data.size());
    copy(data.begin(), data.end(), &c[0]);
    checkColorSize(type, c);
    if(type == spectrum)
    {
      c.unit = parseSpectralUnitAttrib(elem.atts["unit"]);
      int values;
      parse(elem.atts["values"].c_str(), values);
      vector<double> wavelengths;
      parse(elem.atts["wavelengths"].c_str(), wavelengths);
      if(wavelengths.size() > 1)
      {
        c.wavelength = wavelengths[0];
        c.step_size = (wavelengths[1] - wavelengths[0])/static_cast<double>(values - 1);
      }
    }
  }
}

void handle_Material(XmlElement& elem)
{
  name = elem.atts["name"];
  current = &(*media)[name];
  current->name = name;

  Interface* iface = &(*interfaces)[name];
  iface->name = name;
  iface->med_in = current;

  elem.process_elements();
}

void handle_RefractiveIndex(XmlElement& elem)
{
  ColorType type = parseColorAttrib(elem.atts["color"]);
  Color< complex<double> >& c = current->get_ior(type);
  vector<double> data;
  parse(elem.body->text.c_str(), data);
  c.resize(data.size()/2);
  for(unsigned int i = 0; i < c.size(); ++i)
    c[i] = complex<double>(data[i*2], data[i*2 + 1]);
  checkColorSize(type, c);
  if(type == spectrum)
  {
    c.unit = parseSpectralUnitAttrib(elem.atts["unit"]);
    int values;
    parse(elem.atts["values"].c_str(), values);
    vector<double> wavelengths;
    parse(elem.atts["wavelengths"].c_str(), wavelengths);
    if(wavelengths.size() > 1)
    {
      c.wavelength = wavelengths[0];
      c.step_size = (wavelengths[1] - wavelengths[0])/static_cast<double>(values - 1);
    }
  }
}

void handle_Scattering(XmlElement& elem)
{
  current->turbid = true;
  current->scatter_unit = parseScatteringUnitAttrib(elem.atts["unit"]);
  elem.process_elements();
}

void handle_Coefficient(XmlElement& elem)
{
  ColorType type = parseColorAttrib(elem.atts["color"]);
  parseColorData(elem, type, current->get_scattering(type));
}

void handle_Asymmetry(XmlElement& elem)
{
  ColorType type = parseColorAttrib(elem.atts["color"]);
  parseColorData(elem, type, current->get_asymmetry(type));
}

void handle_AngularMap(XmlElement& elem)
{
  
}

void handle_Interface(XmlElement& elem)
{
  name = elem.atts["name"];
  Interface* iface = &(*interfaces)[name];
  iface->name = name;
  iface->med_in = &(*media)[elem.atts["inside"]];
  iface->med_out = &(*media)[elem.atts["outside"]];
}

void load_mpml(string filename, map<string, Medium>& media_map, map<string, Interface>& interface_map)
{
  cout << "Loading " << filename << endl;
  media = &media_map;
  interfaces = &interface_map;
  XmlDoc mpml(filename.c_str());
  mpml.add_handler("Material", handle_Material);
  mpml.add_handler("RefractiveIndex", handle_RefractiveIndex);
  mpml.add_handler("Scattering", handle_Scattering);
  mpml.add_handler("Coefficient", handle_Coefficient);
  mpml.add_handler("Asymmetry", handle_Asymmetry);
  mpml.add_handler("Interface", handle_Interface);
  mpml.process_elements();
  mpml.close();
}
