#ifndef INCLUDE_MINFTGL_H
#define INCLUDE_MINFTGL_H
#include <cwchar>
#include <string>

/**
 *  @brief Some hiden data in these structure 
 */
struct preFace;
struct preData;

namespace minftgl{
    
    struct Color{ int r, g, b; };
     
    class Font{
    public:

        /**
         *  @brief Add font in Freetype database
         *  @param
         *      font_path : path of truetype font file
         *      height : character height pixel
         */
        Font(const char* font_path, int height = 60);
        
        ~Font();
        
        friend class Label;
 
    private:
        preFace* face;
    };
    
    class Label{
    public:
        
        /**
         *  @brief Apply *font* to Label
         *  @param
         *      str : the string of Label
         *      font : the font of Label
         *      color : the color of Label
         */
        Label(const wchar_t* str, Font* font, Color color = {0, 0, 0});

        ~Label();
        
        /**
         *  @brief Render the label with coordinate
         *  @param
         *      left : How long distance from left 
         *      top : How long distance from top
         */
        void Render(double left, double top);
    
    private:    
        preData* data;
    }; 

    /**
     *  @brief Initial the minftgl envirnment 
     */    
    void Init();
};

#endif
