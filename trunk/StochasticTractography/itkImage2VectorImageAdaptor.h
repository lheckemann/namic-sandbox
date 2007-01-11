  /** Connect a DWI Image **/
  void AddDWIImage(TDWIImage* image,
    TGradientDirection direction,
    TbValue bvalue);
  
  /** Connect a DWI Image without specifying a b value **/
  void AddDWIImageNobValue(TDWIImage* image,
    TGradientDirection direction);
