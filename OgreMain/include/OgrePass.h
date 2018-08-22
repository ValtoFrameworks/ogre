/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __Pass_H__
#define __Pass_H__

#include "OgrePrerequisites.h"
#include "OgreColourValue.h"
#include "OgreCommon.h"
#include "OgreLight.h"
#include "OgreTextureUnitState.h"
#include "OgreUserObjectBindings.h"
#include "OgreHeaderPrefix.h"

namespace Ogre {

    /** \addtogroup Core
     *  @{
     */
    /** \addtogroup Materials
     *  @{
     */
    /// Categorisation of passes for the purpose of additive lighting
    enum IlluminationStage
    {
        /// Part of the rendering which occurs without any kind of direct lighting
        IS_AMBIENT,
        /// Part of the rendering which occurs per light
        IS_PER_LIGHT,
        /// Post-lighting rendering
        IS_DECAL,
        /// Not determined
        IS_UNKNOWN
    };

    /** Class defining a single pass of a Technique (of a Material), i.e.
        a single rendering call.
        @remarks
        Rendering can be repeated with many passes for more complex effects.
        Each pass is either a fixed-function pass (meaning it does not use
        a vertex or fragment program) or a programmable pass (meaning it does
        use either a vertex and fragment program, or both).
        @par
        Programmable passes are complex to define, because they require custom
        programs and you have to set all constant inputs to the programs (like
        the position of lights, any base material colours you wish to use etc), but
        they do give you much total flexibility over the algorithms used to render your
        pass, and you can create some effects which are impossible with a fixed-function pass.
        On the other hand, you can define a fixed-function pass in very little time, and
        you can use a range of fixed-function effects like environment mapping very
        easily, plus your pass will be more likely to be compatible with older hardware.
        There are pros and cons to both, just remember that if you use a programmable
        pass to create some great effects, allow more time for definition and testing.
    */
    class _OgreExport Pass : public PassAlloc
    {
    public:
        /** Definition of a functor for calculating the hashcode of a Pass.
            @remarks
            The hashcode of a Pass is used to sort Passes for rendering, in order
            to reduce the number of render state changes. Each Pass represents a
            single unique set of states, but by ordering them, state changes can
            be minimised between passes. An implementation of this functor should
            order passes so that the elements that you want to keep constant are
            sorted next to each other.

            Hash format is 32-bit, divided as follows (high to low bits)
            bits   purpose
             4     Pass index (i.e. max 16 passes!).
            28     Pass contents

            @note the high bits returned by this function will get overwritten

            @see Pass::setHashFunc
        */
        struct HashFunc
        {
            virtual uint32 operator()(const Pass* p) const = 0;
            /// Need virtual destructor in case subclasses use it
            virtual ~HashFunc() {}
        };

        typedef std::vector<TextureUnitState*> TextureUnitStates;
    protected:
        Technique* mParent;
        String mName; /// Optional name for the pass
        uint32 mHash; /// Pass hash
        ushort mIndex; /// Pass index
        //-------------------------------------------------------------------------
        // Colour properties, only applicable in fixed-function passes
        ColourValue mAmbient;
        ColourValue mDiffuse;
        ColourValue mSpecular;
        ColourValue mEmissive;
        Real mShininess;
        TrackVertexColourType mTracking;
        //-------------------------------------------------------------------------

        //-------------------------------------------------------------------------
        // Blending factors
        SceneBlendFactor mSourceBlendFactor;
        SceneBlendFactor mDestBlendFactor;
        SceneBlendFactor mSourceBlendFactorAlpha;
        SceneBlendFactor mDestBlendFactorAlpha;

        //-------------------------------------------------------------------------
        // Blending operations
        SceneBlendOperation mBlendOperation;
        SceneBlendOperation mAlphaBlendOperation;

        /// Needs to be dirtied when next loaded
        bool mHashDirtyQueued : 1;
        /// Used to determine if separate alpha blending should be used for color and alpha channels
        bool mSeparateBlend : 1;
        /// Determines if we should use separate blending operations for color and alpha channels
        bool mSeparateBlendOperation : 1;
        /// Colour buffer settings
        bool mColourWriteR : 1;
        bool mColourWriteG : 1;
        bool mColourWriteB : 1;
        bool mColourWriteA : 1;
        // Depth buffer settings
        bool mDepthCheck : 1;
        bool mDepthWrite : 1;
        bool mAlphaToCoverageEnabled : 1;
        /// Transparent depth sorting
        bool mTransparentSorting : 1;
        /// Transparent depth sorting forced
        bool mTransparentSortingForced : 1;
        /// Lighting enabled?
        bool mLightingEnabled : 1;
        /// Run this pass once per light?
        bool mIteratePerLight : 1;
        /// Should it only be run for a certain light type?
        bool mRunOnlyForOneLightType : 1;
        /// Normalisation
        bool mNormaliseNormals : 1;
        bool mPolygonModeOverrideable : 1;
        bool mFogOverride : 1;
        /// Is this pass queued for deletion?
        bool mQueuedForDeletion : 1;
        /// Scissoring for the light?
        bool mLightScissoring : 1;
        /// User clip planes for light?
        bool mLightClipPlanes : 1;
        bool mPointSpritesEnabled : 1;
        bool mPointAttenuationEnabled : 1;
        mutable bool mContentTypeLookupBuilt : 1;

        uchar mAlphaRejectVal;

        CompareFunction mDepthFunc;
        float mDepthBiasConstant;
        float mDepthBiasSlopeScale;
        float mDepthBiasPerIteration;

        // Alpha reject settings
        CompareFunction mAlphaRejectFunc;

        //-------------------------------------------------------------------------

        //-------------------------------------------------------------------------
        // Culling mode
        CullingMode mCullMode;
        ManualCullingMode mManualCullMode;
        //-------------------------------------------------------------------------

        /// Max simultaneous lights
        unsigned short mMaxSimultaneousLights;
        /// Starting light index
        unsigned short mStartLight;
        /// Iterate per how many lights?
        unsigned short mLightsPerIteration;

        Light::LightTypes mOnlyLightType;
        /// With a specific light mask?
        uint32 mLightMask;

        /// Shading options
        ShadeOptions mShadeOptions;
        /// Polygon mode
        PolygonMode mPolygonMode;

        //-------------------------------------------------------------------------
        // Fog
        FogMode mFogMode;
        ColourValue mFogColour;
        Real mFogStart;
        Real mFogEnd;
        Real mFogDensity;
        //-------------------------------------------------------------------------

        /// Storage of texture unit states
        TextureUnitStates mTextureUnitStates;

        /// Vertex program details
        std::unique_ptr<GpuProgramUsage> mProgramUsage[GPT_COUNT];
        std::unique_ptr<GpuProgramUsage> mShadowCasterVertexProgramUsage;
        std::unique_ptr<GpuProgramUsage> mShadowCasterFragmentProgramUsage;
        std::unique_ptr<GpuProgramUsage> mShadowReceiverVertexProgramUsage;
        std::unique_ptr<GpuProgramUsage> mShadowReceiverFragmentProgramUsage;
        /// Number of pass iterations to perform
        size_t mPassIterationCount;
        /// line width
        float mLineWidth;
        /// Point size, applies when not using per-vertex point size
        Real mPointSize;
        Real mPointMinSize;
        Real mPointMaxSize;
        /// Constant, linear, quadratic coeffs
        Real mPointAttenuationCoeffs[3];
        // TU Content type lookups
        typedef std::vector<unsigned short> ContentTypeLookup;
        mutable ContentTypeLookup mShadowContentTypeLookup;

        /// Illumination stage?
        IlluminationStage mIlluminationStage;
        /// User objects binding.
        UserObjectBindings      mUserObjectBindings;

        /// Used to get scene blending flags from a blending type
        void _getBlendFlags(SceneBlendType type, SceneBlendFactor& source, SceneBlendFactor& dest);

    public:
        typedef std::set<Pass*> PassSet;
    protected:
        /// List of Passes whose hashes need recalculating
        static PassSet msDirtyHashList;
        /// The place where passes go to die
        static PassSet msPassGraveyard;
        /// The Pass hash functor
        static HashFunc* msHashFunc;
    public:
        OGRE_STATIC_MUTEX(msDirtyHashListMutex);
        OGRE_STATIC_MUTEX(msPassGraveyardMutex);
        OGRE_MUTEX(mTexUnitChangeMutex);
        OGRE_MUTEX(mGpuProgramChangeMutex);
        /// Default constructor
        Pass(Technique* parent, unsigned short index);
        /// Copy constructor
        Pass(Technique* parent, unsigned short index, const Pass& oth );

        ~Pass();

        /// Operator = overload
        Pass& operator=(const Pass& oth);

        /// Returns true if this pass is programmable i.e. includes either a vertex or fragment program.
        bool isProgrammable(void) const
        {
            for (const auto& u : mProgramUsage)
                if (u) return true;
            return false;
        }

        /// Returns true if this pass uses a programmable vertex pipeline
        bool hasVertexProgram(void) const { return hasGpuProgram(GPT_VERTEX_PROGRAM); }
        /// Returns true if this pass uses a programmable fragment pipeline
        bool hasFragmentProgram(void) const { return hasGpuProgram(GPT_FRAGMENT_PROGRAM); }
        /// Returns true if this pass uses a programmable geometry pipeline
        bool hasGeometryProgram(void) const { return hasGpuProgram(GPT_GEOMETRY_PROGRAM); }
        /// Returns true if this pass uses a programmable tessellation control pipeline
        bool hasTessellationHullProgram(void) const { return hasGpuProgram(GPT_HULL_PROGRAM); }
        /// Returns true if this pass uses a programmable tessellation control pipeline
        bool hasTessellationDomainProgram(void) const { return hasGpuProgram(GPT_DOMAIN_PROGRAM); }
        /// Returns true if this pass uses a programmable compute pipeline
        bool hasComputeProgram(void) const { return hasGpuProgram(GPT_COMPUTE_PROGRAM); }
        /// Returns true if this pass uses a shadow caster vertex program
        bool hasShadowCasterVertexProgram(void) const { return mShadowCasterVertexProgramUsage != NULL; }
        /// Returns true if this pass uses a shadow caster fragment program
        bool hasShadowCasterFragmentProgram(void) const { return mShadowCasterFragmentProgramUsage != NULL; }
        /// Returns true if this pass uses a shadow receiver vertex program
        bool hasShadowReceiverVertexProgram(void) const { return mShadowReceiverVertexProgramUsage != NULL; }
        /// Returns true if this pass uses a shadow receiver fragment program
        bool hasShadowReceiverFragmentProgram(void) const { return mShadowReceiverFragmentProgramUsage != NULL; }

        size_t calculateSize(void) const;

        /// Gets the index of this Pass in the parent Technique
        unsigned short getIndex(void) const { return mIndex; }
        /** Set the name of the pass
           @remarks
           The name of the pass is optional.  Its useful in material scripts where a material could inherit
           from another material and only want to modify a particular pass.
        */
        void setName(const String& name);
        /// Get the name of the pass
        const String& getName(void) const { return mName; }

        /** Sets the ambient colour reflectance properties of this pass.

        This property determines how much ambient light (directionless global light) is
        reflected. The default is full white, meaning objects are completely globally
        illuminated. Reduce this if you want to see diffuse or specular light effects, or change
        the blend of colours to make the object have a base colour other than white.

        It is also possible to make the ambient reflectance track the vertex colour as defined in
        the mesh instead of the colour values.
        @note
        This setting has no effect if dynamic lighting is disabled (see
        Ogre::Pass::setLightingEnabled), or, if any texture layer has a Ogre::LBO_REPLACE
        attribute.
        */
        void setAmbient(float red, float green, float blue);

        /// @overload
        void setAmbient(const ColourValue& ambient);

        /** Sets the diffuse colour reflectance properties of this pass.

        This property determines how much diffuse light (light from instances
        of the Light class in the scene) is reflected. The default is full white, meaning objects
        reflect the maximum white light they can from Light objects.

        It is also possible to make the diffuse reflectance track the vertex colour as defined in
        the mesh instead of the colour values.
        @note
        This setting has no effect if dynamic lighting is disabled (see
        Ogre::Pass::setLightingEnabled), or, if any texture layer has a Ogre::LBO_REPLACE
        attribute.
        */
        void setDiffuse(float red, float green, float blue, float alpha);

        /// @overload
        void setDiffuse(const ColourValue& diffuse);

        /** Sets the specular colour reflectance properties of this pass.

        This property determines how much specular light (highlights from instances of the Light
        class in the scene) is reflected. The default is to reflect no specular light.

        It is also possible to make the specular reflectance track the vertex colour as defined in
        the mesh instead of the colour values.
        @note
        The size of the specular highlights is determined by the separate 'shininess' property.
        @note
        This setting has no effect if dynamic lighting is disabled (see
        Ogre::Pass::setLightingEnabled), or, if any texture layer has a Ogre::LBO_REPLACE
        attribute.
        */
        void setSpecular(float red, float green, float blue, float alpha);

        /// @overload
        void setSpecular(const ColourValue& specular);

        /** Sets the shininess of the pass, affecting the size of specular highlights.

        The higher the value of the shininess parameter, the sharper the highlight i.e. the radius
        is smaller. Beware of using shininess values in the range of 0 to 1 since this causes the
        the specular colour to be applied to the whole surface that has the material applied to it.
        When the viewing angle to the surface changes, ugly flickering will also occur when
        shininess is in the range of 0 to 1. Shininess values between 1 and 128 work best in both
        DirectX and OpenGL renderers.
        @note
        This setting has no effect if dynamic lighting is disabled (see
        Ogre::Pass::setLightingEnabled), or, if any texture layer has a Ogre::LBO_REPLACE
        attribute.
        */
        void setShininess(Real val);

        /** Sets the amount of self-illumination an object has.

        If an object is self-illuminating, it does not need external sources to light it, ambient or
        otherwise. It's like the object has it's own personal ambient light. This property is rarely useful since
        you can already specify per-pass ambient light, but is here for completeness.

        It is also possible to make the emissive reflectance track the vertex colour as defined in
        the mesh instead of the colour values.
        @note
        This setting has no effect if dynamic lighting is disabled (see
        Ogre::Pass::setLightingEnabled), or, if any texture layer has a Ogre::LBO_REPLACE
        attribute.
        */
        void setSelfIllumination(float red, float green, float blue);

        /// @overload
        void setSelfIllumination(const ColourValue& selfIllum);

        /// @copydoc setSelfIllumination
        void setEmissive(float red, float green, float blue)
        {
            setSelfIllumination(red, green, blue);
        }
        /// @overload
        void setEmissive(const ColourValue& emissive) { setSelfIllumination(emissive); }

        /** Sets which material properties follow the vertex colour
         */
        void setVertexColourTracking(TrackVertexColourType tracking);

        /**
         * set the line width for this pass
         *
         * This property determines what width is used to render lines.
         * @note some drivers only support a value of 1.0 here
         */
        void setLineWidth(float width) { mLineWidth = width; }
        float getLineWidth() const { return mLineWidth; }

        /** Gets the point size of the pass.
            @remarks
            This property determines what point size is used to render a point
            list.
        */
        Real getPointSize(void) const;

        /** Sets the point size of this pass.

            This setting allows you to change the size of points when rendering
            a point list, or a list of point sprites. The interpretation of this
            command depends on the Ogre::Pass::setPointAttenuation option - if it
            is off (the default), the point size is in screen pixels, if it is on,
            it expressed as normalised screen coordinates (1.0 is the height of
            the screen) when the point is at the origin.
            @note
            Some drivers have an upper limit on the size of points they support - this can even vary
            between APIs on the same card! Don't rely on point sizes that cause the point sprites to
            get very large on screen, since they may get clamped on some cards. Upper sizes can range
            from 64 to 256 pixels.
        */
        void setPointSize(Real ps);

        /** Sets whether points will be rendered as textured quads or plain dots

            This setting specifies whether or not hardware point sprite rendering is enabled for
            this pass. Enabling it means that a point list is rendered as a list of quads rather than
            a list of dots. It is very useful to use this option if you are using a BillboardSet and
            only need to use point oriented billboards which are all of the same size. You can also
            use it for any other point list render.
        */
        void setPointSpritesEnabled(bool enabled);

        /** Returns whether point sprites are enabled when rendering a
            point list.
        */
        bool getPointSpritesEnabled(void) const;

        /** Sets how points are attenuated with distance.

            When performing point rendering or point sprite rendering,
            point size can be attenuated with distance. The equation for
            doing this is 
            
            $$attenuation = 1 / (constant + linear * dist + quadratic * d^2)$$

            For example, to disable distance attenuation (constant screensize)
            you would set constant to 1, and linear and quadratic to 0. A
            standard perspective attenuation would be 0, 1, 0 respectively.

            The resulting size is clamped to the minimum and maximum point
            size.
        @param enabled Whether point attenuation is enabled
        @param constant, linear, quadratic Parameters to the attenuation
            function defined above
        */
        void setPointAttenuation(bool enabled,
            Real constant = 0.0f, Real linear = 1.0f, Real quadratic = 0.0f);

        /** Returns whether points are attenuated with distance. */
        bool isPointAttenuationEnabled(void) const;

        /** Returns the constant coefficient of point attenuation. */
        Real getPointAttenuationConstant(void) const;
        /** Returns the linear coefficient of point attenuation. */
        Real getPointAttenuationLinear(void) const;
        /** Returns the quadratic coefficient of point attenuation. */
        Real getPointAttenuationQuadratic(void) const;

        /** Set the minimum point size, when point attenuation is in use. */
        void setPointMinSize(Real min);
        /** Get the minimum point size, when point attenuation is in use. */
        Real getPointMinSize(void) const;
        /** Set the maximum point size, when point attenuation is in use.
            @remarks Setting this to 0 indicates the max size supported by the card.
        */
        void setPointMaxSize(Real max);
        /** Get the maximum point size, when point attenuation is in use.
            @remarks 0 indicates the max size supported by the card.
        */
        Real getPointMaxSize(void) const;

        /** Gets the ambient colour reflectance of the pass.
         */
        const ColourValue& getAmbient(void) const;

        /** Gets the diffuse colour reflectance of the pass.
         */
        const ColourValue& getDiffuse(void) const;

        /** Gets the specular colour reflectance of the pass.
         */
        const ColourValue& getSpecular(void) const;

        /** Gets the self illumination colour of the pass.
         */
        const ColourValue& getSelfIllumination(void) const;

        /** Gets the self illumination colour of the pass.
            @see
            getSelfIllumination
        */
        const ColourValue& getEmissive(void) const
        {
            return getSelfIllumination();
        }

        /** Gets the 'shininess' property of the pass (affects specular highlights).
         */
        Real getShininess(void) const;

        /** Gets which material properties follow the vertex colour
         */
        TrackVertexColourType getVertexColourTracking(void) const;

        /** Inserts a new TextureUnitState object into the Pass.
            @remarks
            This unit is is added on top of all previous units.
            @param textureName
            The basic name of the texture e.g. brickwall.jpg, stonefloor.png
            @param texCoordSet
            The index of the texture coordinate set to use.
            @note
            Applies to both fixed-function and programmable passes.
        */
        TextureUnitState* createTextureUnitState( const String& textureName, unsigned short texCoordSet = 0);
        /// @overload
        TextureUnitState* createTextureUnitState(void);

        /** Adds the passed in TextureUnitState, to the existing Pass.
            @param
            state The Texture Unit State to be attached to this pass.  It must not be attached to another pass.
            @note
            Throws an exception if the TextureUnitState is attached to another Pass.*/
        void addTextureUnitState(TextureUnitState* state);
        /** Retrieves a const pointer to a texture unit state.
         * @deprecated use getTextureUnitStates()
         */
        TextureUnitState* getTextureUnitState(unsigned short index) const;
        /** Retrieves the Texture Unit State matching name.
            Returns 0 if name match is not found.
        */
        TextureUnitState* getTextureUnitState(const String& name) const;


        /**  Retrieve the index of the Texture Unit State in the pass.
             @param
             state The Texture Unit State this is attached to this pass.
             @note
             Throws an exception if the state is not attached to the pass.
             @deprecated use getTextureUnitStates()
        */
        unsigned short getTextureUnitStateIndex(const TextureUnitState* state) const;

        typedef VectorIterator<TextureUnitStates> TextureUnitStateIterator;
        /** Get an iterator over the TextureUnitStates contained in this Pass.
         * @deprecated use getTextureUnitStates() */
        OGRE_DEPRECATED TextureUnitStateIterator getTextureUnitStateIterator(void);

        typedef ConstVectorIterator<TextureUnitStates> ConstTextureUnitStateIterator;
        /** Get an iterator over the TextureUnitStates contained in this Pass.
         * @deprecated use getTextureUnitStates() */
        OGRE_DEPRECATED ConstTextureUnitStateIterator getTextureUnitStateIterator(void) const;

        /** Get the TextureUnitStates contained in this Pass. */
        const TextureUnitStates& getTextureUnitStates() const { return mTextureUnitStates; }

        /** Removes the indexed texture unit state from this pass.
            @remarks
            Note that removing a texture which is not the topmost will have a larger performance impact.
        */
        void removeTextureUnitState(unsigned short index);

        /** Removes all texture unit settings.
         */
        void removeAllTextureUnitStates(void);

        /** Returns the number of texture unit settings.
         * @deprecated use getTextureUnitStates()
         */
        unsigned short getNumTextureUnitStates(void) const
        {
            return static_cast<unsigned short>(mTextureUnitStates.size());
        }

        /** Sets the kind of blending this pass has with the existing contents of the scene.

            Whereas the texture blending operations seen in the TextureUnitState class are concerned with
            blending between texture layers, this blending is about combining the output of the Pass
            as a whole with the existing contents of the rendering target. This blending therefore allows
            object transparency and other special effects. If all passes in a technique have a scene
            blend, then the whole technique is considered to be transparent.

            This method allows you to select one of a number of predefined blending types. If you require more
            control than this, use the alternative version of this method which allows you to specify source and
            destination blend factors.
            @note
            This method is applicable for both the fixed-function and programmable pipelines.
            @param
            sbt One of the predefined SceneBlendType blending types
        */
        void setSceneBlending( const SceneBlendType sbt );

        /** Sets the kind of blending this pass has with the existing contents of the scene, separately for color and alpha channels

            This method allows you to select one of a number of predefined blending types. If you require more
            control than this, use the alternative version of this method which allows you to specify source and
            destination blend factors.

            @param
            sbt One of the predefined SceneBlendType blending types for the color channel
            @param
            sbta One of the predefined SceneBlendType blending types for the alpha channel
        */
        void setSeparateSceneBlending( const SceneBlendType sbt, const SceneBlendType sbta );

        /** Allows very fine control of blending this Pass with the existing contents of the scene.

            This version of the method allows complete control over the blending operation, by specifying the
            source and destination blending factors. The result of the blending operation is:

            $$final = (passOutput * sourceFactor) + (frameBuffer * destFactor)$$

            Each of the factors is specified as one of a number of options, as specified in the Ogre::SceneBlendFactor
            enumerated type.
            @param
            sourceFactor The source factor in the above calculation, i.e. multiplied by the output of the %Pass.
            @param
            destFactor The destination factor in the above calculation, i.e. multiplied by the Frame Buffer contents.
        */
        void setSceneBlending( const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor);

        /** Allows very fine control of blending this Pass with the existing contents of the scene.
            @copydetails Ogre::Pass::setSceneBlending( const SceneBlendFactor, const SceneBlendFactor)
            @param
            sourceFactorAlpha The alpha source factor in the above calculation, i.e. multiplied by the output of the %Pass.
            @param
            destFactorAlpha The alpha destination factor in the above calculation, i.e. multiplied by the Frame Buffer alpha.
        */
        void setSeparateSceneBlending( const SceneBlendFactor sourceFactor, const SceneBlendFactor destFactor, const SceneBlendFactor sourceFactorAlpha, const SceneBlendFactor destFactorAlpha );

        /** Return true if this pass uses separate scene blending */
        bool hasSeparateSceneBlending() const;

        /** Retrieves the source blending factor for the material (as set using Materiall::setSceneBlending).
         */
        SceneBlendFactor getSourceBlendFactor() const;

        /** Retrieves the destination blending factor for the material (as set using Materiall::setSceneBlending).
         */
        SceneBlendFactor getDestBlendFactor() const;

        /** Retrieves the alpha source blending factor for the material (as set using Materiall::setSeparateSceneBlending).
         */
        SceneBlendFactor getSourceBlendFactorAlpha() const;

        /** Retrieves the alpha destination blending factor for the material (as set using Materiall::setSeparateSceneBlending).
         */
        SceneBlendFactor getDestBlendFactorAlpha() const;

        /** Sets the specific operation used to blend source and destination pixels together.

            By default this operation is Ogre::SBO_ADD, which creates this equation

            $$final = (passOutput * sourceFactor) + (frameBuffer * destFactor)$$

            By setting this to something other than SBO_ADD you can change the operation to achieve
            a different effect.
            @param op The blending operation mode to use for this pass
        */
        void setSceneBlendingOperation(SceneBlendOperation op);

        /** Sets the specific operation used to blend source and destination pixels together.

            This function allows more control over blending since it allows you to select different blending
            modes for the color and alpha channels

            @copydetails Pass::setSceneBlendingOperation
            @param alphaOp The blending operation mode to use for alpha channels in this pass
        */
        void setSeparateSceneBlendingOperation(SceneBlendOperation op, SceneBlendOperation alphaOp);

        /** Returns true if this pass uses separate scene blending operations. */
        bool hasSeparateSceneBlendingOperations() const;

        /** Returns the current blending operation */
        SceneBlendOperation getSceneBlendingOperation() const;

        /** Returns the current alpha blending operation */
        SceneBlendOperation getSceneBlendingOperationAlpha() const;

        /** Returns true if this pass has some element of transparency. */
        bool isTransparent(void) const;

        /** Sets whether or not this pass renders with depth-buffer checking on or not.

            If depth-buffer checking is on, whenever a pixel is about to be written to the frame buffer
            the depth buffer is checked to see if the pixel is in front of all other pixels written at that
            point. If not, the pixel is not written.

            If depth checking is off, pixels are written no matter what has been rendered before.
            Also see setDepthFunction for more advanced depth check configuration.
            @see Ogre::CompareFunction
        */
        void setDepthCheckEnabled(bool enabled);

        /** Returns whether or not this pass renders with depth-buffer checking on or not.
        */
        bool getDepthCheckEnabled(void) const;

        /** Sets whether or not this pass renders with depth-buffer writing on or not.

            If depth-buffer writing is on, whenever a pixel is written to the frame buffer
            the depth buffer is updated with the depth value of that new pixel, thus affecting future
            rendering operations if future pixels are behind this one.

            If depth writing is off, pixels are written without updating the depth buffer Depth writing should
            normally be on but can be turned off when rendering static backgrounds or when rendering a collection
            of transparent objects at the end of a scene so that they overlap each other correctly.
        */
        void setDepthWriteEnabled(bool enabled);

        /** Returns whether or not this pass renders with depth-buffer writing on or not.
        */
        bool getDepthWriteEnabled(void) const;

        /** Sets the function used to compare depth values when depth checking is on.

            If depth checking is enabled (see setDepthCheckEnabled) a comparison occurs between the depth
            value of the pixel to be written and the current contents of the buffer. This comparison is
            normally Ogre::CMPF_LESS_EQUAL.
        */
        void setDepthFunction( CompareFunction func );
        /** Returns the function used to compare depth values when depth checking is on.
            @see
            setDepthFunction
        */
        CompareFunction getDepthFunction(void) const;

        /** Sets whether or not colour buffer writing is enabled for this %Pass.

            If colour writing is off no visible pixels are written to the screen during this pass.
            You might think this is useless, but if you render with colour writing off, and with very
            minimal other settings, you can use this pass to initialise the depth buffer before
            subsequently rendering other passes which fill in the colour data. This can give you
            significant performance boosts on some newer cards, especially when using complex
            fragment programs, because if the depth check fails then the fragment program is never
            run.
        */
        void setColourWriteEnabled(bool enabled);
        /** Determines if colour buffer writing is enabled for this pass i.e. when at least one
            colour channel is enabled for writing.
         */
        bool getColourWriteEnabled(void) const;

        /// Sets which colour buffer channels are enabled for writing for this Pass
        void setColourWriteEnabled(bool red, bool green, bool blue, bool alpha);
        /// Determines which colour buffer channels are enabled for writing for this pass.
        void getColourWriteEnabled(bool& red, bool& green, bool& blue, bool& alpha) const;

        /** Sets the culling mode for this pass based on the 'vertex winding'.
            A typical way for the rendering engine to cull triangles is based on the 'vertex winding' of
            triangles. Vertex winding refers to the direction in which the vertices are passed or indexed
            to in the rendering operation as viewed from the camera, and will wither be clockwise or
            anticlockwise (that's 'counterclockwise' for you Americans out there ;) The default is
            Ogre::CULL_CLOCKWISE i.e. that only triangles whose vertices are passed/indexed in anticlockwise order
            are rendered - this is a common approach and is used in 3D studio models for example. You can
            alter this culling mode if you wish but it is not advised unless you know what you are doing.

            You may wish to use the Ogre::CULL_NONE option for mesh data that you cull yourself where the vertex
            winding is uncertain or for creating 2-sided passes.
        */
        void setCullingMode( CullingMode mode );

        /** Returns the culling mode for geometry rendered with this pass. See setCullingMode for more information.
         */
        CullingMode getCullingMode(void) const;

        /** Sets the manual culling mode, performed by CPU rather than hardware.

            In some situations you want to use manual culling of triangles rather than sending the
            triangles to the hardware and letting it cull them. This setting only takes effect on
            SceneManager's that use it (since it is best used on large groups of planar world geometry rather
            than on movable geometry since this would be expensive), but if used can cull geometry before it is
            sent to the hardware.

            In this case the culling is based on whether the ’back’ or ’front’ of the triangle is facing the
            camera - this definition is based on the face normal (a vector which sticks out of the front side of
            the polygon perpendicular to the face). Since %Ogre expects face normals to be on anticlockwise side
            of the face, Ogre::MANUAL_CULL_BACK is the software equivalent of Ogre::CULL_CLOCKWISE setting,
            which is why they are both the default. The naming is different to reflect the way the culling is
            done though, since most of the time face normals are pre-calculated and they don’t have to be the
            way %Ogre expects - you could set Ogre::CULL_NONE and completely cull in software based on your
            own face normals, if you have the right SceneManager which uses them.
        */
        void setManualCullingMode( ManualCullingMode mode );

        /** Retrieves the manual culling mode for this pass
            @see
            setManualCullingMode
        */
        ManualCullingMode getManualCullingMode(void) const;

        /** Sets whether or not dynamic lighting is enabled.

            Turning dynamic lighting off makes any ambient, diffuse, specular, emissive and shading
            properties for this pass redundant. 
            If lighting is turned off, all objects rendered using the pass will be fully lit.
            When lighting is turned on, objects are lit according
            to their vertex normals for diffuse and specular light, and globally for ambient and
            emissive.
        */
        void setLightingEnabled(bool enabled);

        /** Returns whether or not dynamic lighting is enabled.
         */
        bool getLightingEnabled(void) const;

        /** Sets the maximum number of lights to be used by this pass.
            @remarks
            During rendering, if lighting is enabled (or if the pass uses an automatic
            program parameter based on a light) the engine will request the nearest lights
            to the object being rendered in order to work out which ones to use. This
            parameter sets the limit on the number of lights which should apply to objects
            rendered with this pass.
        */
        void setMaxSimultaneousLights(unsigned short maxLights);
        /** Gets the maximum number of lights to be used by this pass. */
        unsigned short getMaxSimultaneousLights(void) const;

        /** Sets the light index that this pass will start at in the light list.

            Normally the lights passed to a pass will start from the beginning
            of the light list for this object. This option allows you to make this
            pass start from a higher light index, for example if one of your earlier
            passes could deal with lights 0-3, and this pass dealt with lights 4+.
            This option also has an interaction with pass iteration, in that
            if you choose to iterate this pass per light too, the iteration will
            only begin from light 4.
        */
        void setStartLight(unsigned short startLight);
        /** Gets the light index that this pass will start at in the light list. */
        unsigned short getStartLight(void) const;

        /** Sets the light mask which can be matched to specific light flags to be handled by this pass */
        void setLightMask(uint32 mask);
        /** Gets the light mask controlling which lights are used for this pass */
        uint32 getLightMask() const;

        /** Sets the type of light shading required

            When dynamic lighting is turned on, the effect is to generate colour values at each
            vertex. Whether these values are interpolated across the face (and how) depends on this
            setting. The default shading method is Ogre::SO_GOURAUD.
        */
        void setShadingMode( ShadeOptions mode );

        /** Returns the type of light shading to be used.
         */
        ShadeOptions getShadingMode(void) const;

        /** Sets the type of polygon rendering required
            
            Sets how polygons should be rasterised, i.e. whether they should be filled in, or just drawn as lines or points.
            The default shading method is Ogre::PM_SOLID.
        */
        void setPolygonMode( PolygonMode mode );

        /** Returns the type of light shading to be used.
         */
        PolygonMode getPolygonMode(void) const;

        /** Sets whether the PolygonMode set on this pass can be downgraded by the camera

         @param override If set to false, this pass will always be rendered at its own chosen polygon mode no matter what the
         camera says. The default is true.
        */
        void setPolygonModeOverrideable(bool override)
        {
            mPolygonModeOverrideable = override;
        }

        /** Gets whether this renderable's chosen detail level can be
            overridden (downgraded) by the camera setting.
        */
        bool getPolygonModeOverrideable(void) const
        {
            return mPolygonModeOverrideable;
        }
        /** Sets the fogging mode applied to this pass.
            @remarks
            Fogging is an effect that is applied as polys are rendered. Sometimes, you want
            fog to be applied to an entire scene. Other times, you want it to be applied to a few
            polygons only. This pass-level specification of fog parameters lets you easily manage
            both.
            @par
            The SceneManager class also has a setFog method which applies scene-level fog. This method
            lets you change the fog behaviour for this pass compared to the standard scene-level fog.
            @param
            overrideScene If true, you authorise this pass to override the scene's fog params with it's own settings.
            If you specify false, so other parameters are necessary, and this is the default behaviour for passes.
            @param
            mode Only applicable if overrideScene is true. You can disable fog which is turned on for the
            rest of the scene by specifying FOG_NONE. Otherwise, set a pass-specific fog mode as
            defined in the enum FogMode.
            @param
            colour The colour of the fog. Either set this to the same as your viewport background colour,
            or to blend in with a skydome or skybox.
            @param
            expDensity The density of the fog in FOG_EXP or FOG_EXP2 mode, as a value between 0 and 1.
            The default is 0.001.
            @param
            linearStart Distance in world units at which linear fog starts to encroach.
            Only applicable if mode is FOG_LINEAR.
            @param
            linearEnd Distance in world units at which linear fog becomes completely opaque.
            Only applicable if mode is FOG_LINEAR.
        */
        void setFog(
            bool overrideScene,
            FogMode mode = FOG_NONE,
            const ColourValue& colour = ColourValue::White,
            Real expDensity = 0.001, Real linearStart = 0.0, Real linearEnd = 1.0 );

        /** Returns true if this pass is to override the scene fog settings.
         */
        bool getFogOverride(void) const;

        /** Returns the fog mode for this pass.
            @note
            Only valid if getFogOverride is true.
        */
        FogMode getFogMode(void) const;

        /** Returns the fog colour for the scene.
         */
        const ColourValue& getFogColour(void) const;

        /** Returns the fog start distance for this pass.
            @note
            Only valid if getFogOverride is true.
        */
        Real getFogStart(void) const;

        /** Returns the fog end distance for this pass.
            @note
            Only valid if getFogOverride is true.
        */
        Real getFogEnd(void) const;

        /** Returns the fog density for this pass.
            @note
            Only valid if getFogOverride is true.
        */
        Real getFogDensity(void) const;

        /** Sets the depth bias to be used for this material.

            When polygons are coplanar, you can get problems with 'depth fighting' where
            the pixels from the two polys compete for the same screen pixel. This is particularly
            a problem for decals (polys attached to another surface to represent details such as
            bulletholes etc.).

            A way to combat this problem is to use a depth bias to adjust the depth buffer value
            used for the decal such that it is slightly higher than the true value, ensuring that
            the decal appears on top. There are two aspects to the biasing, a constant
            bias value and a slope-relative biasing value, which varies according to the
            maximum depth slope relative to the camera, ie:

            $$finalBias = maxSlope * slopeScaleBias + constantBias$$

            Slope scale biasing is relative to the angle of the polygon to the camera, which makes
            for a more appropriate bias value, but this is ignored on some older hardware. Constant
            biasing is expressed as a factor of the minimum depth value, so a value of 1 will nudge
            the depth by one ’notch’ if you will.
            @param constantBias The constant bias value
            @param slopeScaleBias The slope-relative bias value
        */
        void setDepthBias(float constantBias, float slopeScaleBias = 0.0f);

        /** Retrieves the const depth bias value as set by setDepthBias. */
        float getDepthBiasConstant(void) const;
        /** Retrieves the slope-scale depth bias value as set by setDepthBias. */
        float getDepthBiasSlopeScale(void) const;
        /** Sets a factor which derives an additional depth bias from the number
            of times a pass is iterated.
            @remarks
            The Final depth bias will be the constant depth bias as set through
            setDepthBias, plus this value times the iteration number.
        */
        void setIterationDepthBias(float biasPerIteration);
        /** Gets a factor which derives an additional depth bias from the number
            of times a pass is iterated.
        */
        float getIterationDepthBias() const;

        /** Sets the way the pass will have use alpha to totally reject pixels from the pipeline.
            @remarks
            The default is CMPF_ALWAYS_PASS i.e. alpha is not used to reject pixels.
            @param func The comparison which must pass for the pixel to be written.
            @param value 1 byte value against which alpha values will be tested(0-255)
            @param alphaToCoverageEnabled Whether to enable alpha to coverage support
            @note
            This option applies in both the fixed function and the programmable pipeline.
        */
        void setAlphaRejectSettings(CompareFunction func, unsigned char value, bool alphaToCoverageEnabled = false);

        /** Sets the alpha reject function. @see setAlphaRejectSettings for more information.
         */
        void setAlphaRejectFunction(CompareFunction func);

        /** Gets the alpha reject value. @see setAlphaRejectSettings for more information.
         */
        void setAlphaRejectValue(unsigned char val);

        /** Gets the alpha reject function. @see setAlphaRejectSettings for more information.
         */
        CompareFunction getAlphaRejectFunction(void) const { return mAlphaRejectFunc; }

        /** Gets the alpha reject value. @see setAlphaRejectSettings for more information.
         */
        unsigned char getAlphaRejectValue(void) const { return mAlphaRejectVal; }

        /** Sets whether to use alpha to coverage (A2C) when blending alpha rejected values.

            Alpha to coverage performs multisampling on the edges of alpha-rejected
            textures to produce a smoother result. It is only supported when multisampling
            is already enabled on the render target, and when the hardware supports
            alpha to coverage (see RenderSystemCapabilities).
            The common use for alpha to coverage is foliage rendering and chain-link fence style
            textures.
        */
        void setAlphaToCoverageEnabled(bool enabled);

        /** Gets whether to use alpha to coverage (A2C) when blending alpha rejected values.
         */
        bool isAlphaToCoverageEnabled() const { return mAlphaToCoverageEnabled; }

        /** Sets whether or not transparent sorting is enabled.
            @param enabled
            If false depth sorting of this material will be disabled.
            @remarks
            By default all transparent materials are sorted such that renderables furthest
            away from the camera are rendered first. This is usually the desired behaviour
            but in certain cases this depth sorting may be unnecessary and undesirable. If
            for example it is necessary to ensure the rendering order does not change from
            one frame to the next.
            @note
            This will have no effect on non-transparent materials.
        */
        void setTransparentSortingEnabled(bool enabled);

        /** Returns whether or not transparent sorting is enabled.
         */
        bool getTransparentSortingEnabled(void) const;

        /** Sets whether or not transparent sorting is forced.
            @param enabled
            If true depth sorting of this material will be depend only on the value of
            getTransparentSortingEnabled().
            @remarks
            By default even if transparent sorting is enabled, depth sorting will only be
            performed when the material is transparent and depth write/check are disabled.
            This function disables these extra conditions.
        */
        void setTransparentSortingForced(bool enabled);

        /** Returns whether or not transparent sorting is forced.
         */
        bool getTransparentSortingForced(void) const;

        /** Sets whether or not this pass should iterate per light or number of
            lights which can affect the object being rendered.
            @remarks
            The default behaviour for a pass (when this option is 'false'), is
            for a pass to be rendered only once (or the number of times set in
            setPassIterationCount), with all the lights which could
            affect this object set at the same time (up to the maximum lights
            allowed in the render system, which is typically 8).
            @par
            Setting this option to 'true' changes this behaviour, such that
            instead of trying to issue render this pass once per object, it
            is run <b>per light</b>, or for a group of 'n' lights each time
            which can affect this object, the number of
            times set in setPassIterationCount (default is once). In
            this case, only light index 0 is ever used, and is a different light
            every time the pass is issued, up to the total number of lights
            which is affecting this object. This has 2 advantages:
            <ul><li>There is no limit on the number of lights which can be
            supported</li>
            <li>It's easier to write vertex / fragment programs for this because
            a single program can be used for any number of lights</li>
            </ul>
            However, this technique is more expensive, and typically you
            will want an additional ambient pass, because if no lights are
            affecting the object it will not be rendered at all, which will look
            odd even if ambient light is zero (imagine if there are lit objects
            behind it - the objects silhouette would not show up). Therefore,
            use this option with care, and you would be well advised to provide
            a less expensive fallback technique for use in the distance.
            @note
            The number of times this pass runs is still limited by the maximum
            number of lights allowed as set in setMaxSimultaneousLights, so
            you will never get more passes than this. Also, the iteration is
            started from the 'start light' as set in Pass::setStartLight, and
            the number of passes is the number of lights to iterate over divided
            by the number of lights per iteration (default 1, set by
            setLightCountPerIteration).
            @param enabled Whether this feature is enabled
            @param onlyForOneLightType If true, the pass will only be run for a single type
            of light, other light types will be ignored.
            @param lightType The single light type which will be considered for this pass
        */
        void setIteratePerLight(bool enabled,
                                bool onlyForOneLightType = true, Light::LightTypes lightType = Light::LT_POINT);

        /** Does this pass run once for every light in range? */
        bool getIteratePerLight(void) const { return mIteratePerLight; }
        /** Does this pass run only for a single light type (if getIteratePerLight is true). */
        bool getRunOnlyForOneLightType(void) const { return mRunOnlyForOneLightType; }
        /** Gets the single light type this pass runs for if  getIteratePerLight and
            getRunOnlyForOneLightType are both true. */
        Light::LightTypes getOnlyLightType() const { return mOnlyLightType; }

        /** If light iteration is enabled, determine the number of lights per
            iteration.
            @remarks
            The default for this setting is 1, so if you enable light iteration
            (Pass::setIteratePerLight), the pass is rendered once per light. If
            you set this value higher, the passes will occur once per 'n' lights.
            The start of the iteration is set by Pass::setStartLight and the end
            by Pass::setMaxSimultaneousLights.
        */
        void setLightCountPerIteration(unsigned short c);
        /** If light iteration is enabled, determine the number of lights per
            iteration.
        */
        unsigned short getLightCountPerIteration(void) const;

        /// Gets the parent Technique
        Technique* getParent(void) const { return mParent; }

        /// Gets the resource group of the ultimate parent Material
        const String& getResourceGroup(void) const;

        /// Gets the Gpu program used by this pass, only available after _load()
        const GpuProgramPtr& getGpuProgram(GpuProgramType programType) const;
        /// @overload
        const GpuProgramPtr& getVertexProgram(void) const;
        /// @overload
        const GpuProgramPtr& getFragmentProgram(void) const;
        /// @overload
        const GpuProgramPtr& getGeometryProgram(void) const;
        /// @overload
        const GpuProgramPtr& getTessellationHullProgram(void) const;
        /// @overload
        const GpuProgramPtr& getTessellationDomainProgram(void) const;
        /// @overload
        const GpuProgramPtr& getComputeProgram(void) const;

        bool hasGpuProgram(GpuProgramType programType) const;

        /** Sets the details of the vertex program to use when rendering as a
            shadow caster.
            @remarks
            Texture-based shadows require that the caster is rendered to a texture
            in a solid colour (the shadow colour in the case of modulative texture
            shadows). Whilst Ogre can arrange this for the fixed function
            pipeline, passes which use vertex programs might need the vertex
            programs still to run in order to preserve any deformation etc
            that it does. However, lighting calculations must be a lot simpler,
            with only the ambient colour being used (which the engine will ensure
            is bound to the shadow colour).
            @par
            Therefore, it is up to implementors of vertex programs to provide an
            alternative vertex program which can be used to render the object
            to a shadow texture. Do all the same vertex transforms, but set the
            colour of the vertex to the ambient colour, as bound using the
            standard auto parameter binding mechanism.
            @note
            Some vertex programs will work without doing this, because Ogre ensures
            that all lights except for ambient are set black. However, the chances
            are that your vertex program is doing a lot of unnecessary work in this
            case, since the other lights are having no effect, and it is good practice
            to supply an alternative.
            @note
            This is only applicable to programmable passes.
            @par
            The default behaviour is for Ogre to switch to fixed-function
            rendering if an explicit vertex program alternative is not set.
        */
        void setShadowCasterVertexProgram(const String& name);
        /** Sets the vertex program parameters for rendering as a shadow caster.
            @remarks
            Only applicable to programmable passes, and this particular call is
            designed for low-level programs; use the named parameter methods
            for setting high-level program parameters.
        */
        void setShadowCasterVertexProgramParameters(GpuProgramParametersSharedPtr params);
        /** Gets the name of the vertex program used by this pass when rendering shadow casters. */
        const String& getShadowCasterVertexProgramName(void) const;
        /** Gets the vertex program parameters used by this pass when rendering shadow casters. */
        GpuProgramParametersSharedPtr getShadowCasterVertexProgramParameters(void) const;
        /** Gets the vertex program used by this pass when rendering shadow casters,
            only available after _load(). */
        const GpuProgramPtr& getShadowCasterVertexProgram(void) const;

        /** Sets the details of the fragment program to use when rendering as a
            shadow caster.
            @remarks
            Texture-based shadows require that the caster is rendered to a texture
            in a solid colour (the shadow colour in the case of modulative texture
            shadows). Whilst Ogre can arrange this for the fixed function
            pipeline, passes which use vertex programs might need the vertex
            programs still to run in order to preserve any deformation etc
            that it does. However, lighting calculations must be a lot simpler,
            with only the ambient colour being used (which the engine will ensure
            is bound to the shadow colour).
            @par
            Therefore, it is up to implementors of vertex programs to provide an
            alternative vertex program which can be used to render the object
            to a shadow texture. Do all the same vertex transforms, but set the
            colour of the vertex to the ambient colour, as bound using the
            standard auto parameter binding mechanism.
            @note
            Some vertex programs will work without doing this, because Ogre ensures
            that all lights except for ambient are set black. However, the chances
            are that your vertex program is doing a lot of unnecessary work in this
            case, since the other lights are having no effect, and it is good practice
            to supply an alternative.
            @note
            This is only applicable to programmable passes.
            @par
            The default behaviour is for Ogre to switch to fixed-function
            rendering if an explicit fragment program alternative is not set.
        */
        void setShadowCasterFragmentProgram(const String& name);
        /** Sets the fragment program parameters for rendering as a shadow caster.
            @remarks
            Only applicable to programmable passes, and this particular call is
            designed for low-level programs; use the named parameter methods
            for setting high-level program parameters.
        */
        void setShadowCasterFragmentProgramParameters(GpuProgramParametersSharedPtr params);
        /** Gets the name of the fragment program used by this pass when rendering shadow casters. */
        const String& getShadowCasterFragmentProgramName(void) const;
        /** Gets the fragment program parameters used by this pass when rendering shadow casters. */
        GpuProgramParametersSharedPtr getShadowCasterFragmentProgramParameters(void) const;
        /** Gets the fragment program used by this pass when rendering shadow casters,
            only available after _load(). */
        const GpuProgramPtr& getShadowCasterFragmentProgram(void) const;

        /** Sets the details of the vertex program to use when rendering as a
            shadow receiver.
            @remarks
            Texture-based shadows require that the shadow receiver is rendered using
            a projective texture. Whilst Ogre can arrange this for the fixed function
            pipeline, passes which use vertex programs might need the vertex
            programs still to run in order to preserve any deformation etc
            that it does. So in this case, we need a vertex program which does the
            appropriate vertex transformation, but generates projective texture
            coordinates.
            @par
            Therefore, it is up to implementors of vertex programs to provide an
            alternative vertex program which can be used to render the object
            as a shadow receiver. Do all the same vertex transforms, but generate
            <strong>2 sets</strong> of texture coordinates using the auto parameter
            ACT_TEXTURE_VIEWPROJ_MATRIX, which Ogre will bind to the parameter name /
            index you supply as the second parameter to this method. 2 texture
            sets are needed because Ogre needs to use 2 texture units for some
            shadow effects.
            @note
            This is only applicable to programmable passes.
            @par
            The default behaviour is for Ogre to switch to fixed-function
            rendering if an explict vertex program alternative is not set.
        */
        void setShadowReceiverVertexProgram(const String& name);
        /** Sets the vertex program parameters for rendering as a shadow receiver.
            @remarks
            Only applicable to programmable passes, and this particular call is
            designed for low-level programs; use the named parameter methods
            for setting high-level program parameters.
        */
        void setShadowReceiverVertexProgramParameters(GpuProgramParametersSharedPtr params);

        /** This method allows you to specify a fragment program for use when
            rendering a texture shadow receiver.
            @remarks
            Texture shadows are applied by rendering the receiver. Modulative texture
            shadows are performed as a post-render darkening pass, and as such
            fragment programs are generally not required per-object. Additive
            texture shadows, however, are applied by accumulating light masked
            out using a texture shadow (black & white by default, unless you
            customise this using SceneManager::setCustomShadowCasterMaterial).
            OGRE can do this for you for most materials, but if you use a custom
            lighting program (e.g. per pixel lighting) then you'll need to provide
            a custom version for receiving shadows. You don't need to provide
            this for shadow casters if you don't use self-shadowing since they
            will never be shadow receivers too.
            @par
            The shadow texture is always bound to texture unit 0 when rendering
            texture shadow passes. Therefore your custom shadow receiver program
            may well just need to shift it's texture unit usage up by one unit,
            and take the shadow texture into account in its calculations.
        */
        void setShadowReceiverFragmentProgram(const String& name);
        /** Sets the fragment program parameters for rendering as a shadow receiver.
            @remarks
            Only applicable to programmable passes, and this particular call is
            designed for low-level programs; use the named parameter methods
            for setting high-level program parameters.
        */
        void setShadowReceiverFragmentProgramParameters(GpuProgramParametersSharedPtr params);

        /** Gets the name of the vertex program used by this pass when rendering shadow receivers. */
        const String& getShadowReceiverVertexProgramName(void) const;
        /** Gets the vertex program parameters used by this pass when rendering shadow receivers. */
        GpuProgramParametersSharedPtr getShadowReceiverVertexProgramParameters(void) const;
        /** Gets the vertex program used by this pass when rendering shadow receivers,
            only available after _load(). */
        const GpuProgramPtr& getShadowReceiverVertexProgram(void) const;

        /** Gets the name of the fragment program used by this pass when rendering shadow receivers. */
        const String& getShadowReceiverFragmentProgramName(void) const;
        /** Gets the fragment program parameters used by this pass when rendering shadow receivers. */
        GpuProgramParametersSharedPtr getShadowReceiverFragmentProgramParameters(void) const;
        /** Gets the fragment program used by this pass when rendering shadow receivers,
            only available after _load(). */
        const GpuProgramPtr& getShadowReceiverFragmentProgram(void) const;

        /** Sets the details of the program to use.
            @remarks
            Only applicable to programmable passes, this sets the details of
            the program to use in this pass. The program will not be
            loaded until the parent Material is loaded.
            @param resetParams
            If true, this will create a fresh set of parameters from the
            new program being linked, so if you had previously set parameters
            you will have to set them again. If you set this to false, you must
            be absolutely sure that the parameters match perfectly, and in the
            case of named parameters refers to the indexes underlying them,
            not just the names.
        */
        void setGpuProgram(GpuProgramType type, const GpuProgramPtr& prog, bool resetParams = true);
        /** @overload
            @param name The name of the program - this must have been
            created using GpuProgramManager by the time that this Pass
            is loaded. If this parameter is blank, any program of the type in this pass is disabled.
        */
        void setGpuProgram(GpuProgramType type, const String& name, bool resetParams = true);
        /// @overload
        void setFragmentProgram(const String& name, bool resetParams = true);
        /// @overload
        void setGeometryProgram(const String& name, bool resetParams = true);
        /// @overload
        void setTessellationDomainProgram(const String& name, bool resetParams = true);
        /// @overload
        void setTessellationHullProgram(const String& name, bool resetParams = true);
        /// @overload
        void setVertexProgram(const String& name, bool resetParams = true);
        /// @overload
        void setComputeProgram(const String& name, bool resetParams = true);

        /** Gets the name of the program used by this pass. */
        const String& getGpuProgramName(GpuProgramType type) const;
        /// @overload
        const String& getFragmentProgramName(void) const { return getGpuProgramName(GPT_FRAGMENT_PROGRAM); }
        /// @overload
        const String& getGeometryProgramName(void) const { return getGpuProgramName(GPT_GEOMETRY_PROGRAM); }
        /// @overload
        const String& getTessellationDomainProgramName(void) const { return getGpuProgramName(GPT_DOMAIN_PROGRAM); }
        /// @overload
        const String& getTessellationHullProgramName(void) const { return getGpuProgramName(GPT_HULL_PROGRAM); }
        /// @overload
        const String& getVertexProgramName(void) const { return getGpuProgramName(GPT_VERTEX_PROGRAM); }
        /// @overload
        const String& getComputeProgramName(void) const { return getGpuProgramName(GPT_COMPUTE_PROGRAM); }

        /** Sets the Gpu program parameters.
            @remarks
            Only applicable to programmable passes, and this particular call is
            designed for low-level programs; use the named parameter methods
            for setting high-level program parameters.
        */
        void setGpuProgramParameters(GpuProgramType type, const GpuProgramParametersSharedPtr& params);
        /// @overload
        void setVertexProgramParameters(GpuProgramParametersSharedPtr params);
        /// @overload
        void setFragmentProgramParameters(GpuProgramParametersSharedPtr params);
        /// @overload
        void setGeometryProgramParameters(GpuProgramParametersSharedPtr params);
        /// @overload
        void setTessellationHullProgramParameters(GpuProgramParametersSharedPtr params);
        /// @overload
        void setTessellationDomainProgramParameters(GpuProgramParametersSharedPtr params);
        /// @overload
        void setComputeProgramParameters(GpuProgramParametersSharedPtr params);

        /** Gets the Gpu program parameters used by this pass. */
        const GpuProgramParametersSharedPtr& getGpuProgramParameters(GpuProgramType type) const;
        /// @overload
        GpuProgramParametersSharedPtr getVertexProgramParameters(void) const;
        /// @overload
        GpuProgramParametersSharedPtr getFragmentProgramParameters(void) const;
        /// @overload
        GpuProgramParametersSharedPtr getGeometryProgramParameters(void) const;
        /// @overload
        GpuProgramParametersSharedPtr getTessellationHullProgramParameters(void) const;
        /// @overload
        GpuProgramParametersSharedPtr getTessellationDomainProgramParameters(void) const;
        /// @overload
        GpuProgramParametersSharedPtr getComputeProgramParameters(void) const;

        /** Splits this Pass to one which can be handled in the number of
            texture units specified.
            @remarks
            Only works on non-programmable passes, programmable passes cannot be
            split, it's up to the author to ensure that there is a fallback Technique
            for less capable cards.
            @param numUnits The target number of texture units
            @return A new Pass which contains the remaining units, and a scene_blend
            setting appropriate to approximate the multitexture. This Pass will be
            attached to the parent Technique of this Pass.
        */
        Pass* _split(unsigned short numUnits);

        /** Internal method to adjust pass index. */
        void _notifyIndex(unsigned short index);

        /** Internal method for preparing to load this pass. */
        void _prepare(void);
        /** Internal method for undoing the load preparartion for this pass. */
        void _unprepare(void);
        /** Internal method for loading this pass. */
        void _load(void);
        /** Internal method for unloading this pass. */
        void _unload(void);
        /// Is this loaded?
        bool isLoaded(void) const;

        /** Gets the 'hash' of this pass, ie a precomputed number to use for sorting
            @remarks
            This hash is used to sort passes, and for this reason the pass is hashed
            using firstly its index (so that all passes are rendered in order), then
            by the textures which it's TextureUnitState instances are using.
        */
        uint32 getHash(void) const { return mHash; }
        /// Mark the hash as dirty
        void _dirtyHash(void);
        /** Internal method for recalculating the hash.
            @remarks
            Do not call this unless you are sure the old hash is not still being
            used by anything. If in doubt, call _dirtyHash if you want to force
            recalculation of the has next time.
        */
        void _recalculateHash(void);
        /** Tells the pass that it needs recompilation. */
        void _notifyNeedsRecompile(void);

        /** Update automatic parameters.
            @param source The source of the parameters
            @param variabilityMask A mask of GpuParamVariability which identifies which autos will need updating
        */
        void _updateAutoParams(const AutoParamDataSource* source, uint16 variabilityMask) const;

        /** Gets the 'nth' texture which references the given content type.
            @remarks
            If the 'nth' texture unit which references the content type doesn't
            exist, then this method returns an arbitrary high-value outside the
            valid range to index texture units.
        */
        unsigned short _getTextureUnitWithContentTypeIndex(
            TextureUnitState::ContentType contentType, unsigned short index) const;

        /** Set texture filtering for every texture unit
            @note
            This property actually exists on the TextureUnitState class
            For simplicity, this method allows you to set these properties for
            every current TeextureUnitState, If you need more precision, retrieve the
            TextureUnitState instance and set the property there.
            @see TextureUnitState::setTextureFiltering
        */
        void setTextureFiltering(TextureFilterOptions filterType);
        /** Sets the anisotropy level to be used for all textures.
            @note
            This property has been moved to the TextureUnitState class, which is accessible via the
            Technique and Pass. For simplicity, this method allows you to set these properties for
            every current TeextureUnitState, If you need more precision, retrieve the Technique,
            Pass and TextureUnitState instances and set the property there.
            @see TextureUnitState::setTextureAnisotropy
        */
        void setTextureAnisotropy(unsigned int maxAniso);
        /** If set to true, this forces normals to be normalised dynamically
            by the hardware for this pass.

            This option can be used to prevent lighting variations when scaling an
            object - normally because this scaling is hardware based, the normals
            get scaled too which causes lighting to become inconsistent. By default the
            SceneManager detects scaled objects and does this for you, but
            this has an overhead so you might want to turn that off through
            Ogre::SceneManager::setNormaliseNormalsOnScale(false) and only do it per-Pass
            when you need to.
        */
        void setNormaliseNormals(bool normalise) { mNormaliseNormals = normalise; }

        /** Returns true if this pass has auto-normalisation of normals set. */
        bool getNormaliseNormals(void) const {return mNormaliseNormals; }

        /** Static method to retrieve all the Passes which need their
            hash values recalculated.
        */
        static const PassSet& getDirtyHashList(void)
        { return msDirtyHashList; }
        /** Static method to retrieve all the Passes which are pending deletion.
         */
        static const PassSet& getPassGraveyard(void)
        { return msPassGraveyard; }
        /** Static method to reset the list of passes which need their hash
            values recalculated.
            @remarks
            For performance, the dirty list is not updated progressively as
            the hashes are recalculated, instead we expect the processor of the
            dirty hash list to clear the list when they are done.
        */
        static void clearDirtyHashList(void);

        /** Process all dirty and pending deletion passes. */
        static void processPendingPassUpdates(void);

        /** Queue this pass for deletion when appropriate. */
        void queueForDeletion(void);

        /** Returns whether this pass is ambient only.
         */
        bool isAmbientOnly(void) const;

        /** set the number of iterations that this pass
            should perform when doing fast multi pass operation.
            @remarks
            Only applicable for programmable passes.
            @param count number of iterations to perform fast multi pass operations.
            A value greater than 1 will cause the pass to be executed count number of
            times without changing the render state.  This is very useful for passes
            that use programmable shaders that have to iterate more than once but don't
            need a render state change.  Using multi pass can dramatically speed up rendering
            for materials that do things like fur, blur.
            A value of 1 turns off multi pass operation and the pass does
            the normal pass operation.
        */
        void setPassIterationCount(const size_t count) { mPassIterationCount = count; }

        /** Gets the pass iteration count value.
         */
        size_t getPassIterationCount(void) const { return mPassIterationCount; }

        /** Applies texture names to Texture Unit State with matching texture name aliases.
            All Texture Unit States within the pass are checked.
            If matching texture aliases are found then true is returned.

            @param
            aliasList is a map container of texture alias, texture name pairs
            @param
            apply set true to apply the texture aliases else just test to see if texture alias matches are found.
            @return
            True if matching texture aliases were found in the pass.
        */
        bool applyTextureAliases(const AliasTextureNamePairList& aliasList, const bool apply = true) const;

        /** Sets whether or not this pass will be clipped by a scissor rectangle
            encompassing the lights that are being used in it.

            In order to cut down on fillrate when you have a number of fixed-range
            lights in the scene, you can enable this option to request that
            during rendering, only the region of the screen which is covered by
            the lights is rendered. This region is the screen-space rectangle
            covering the union of the spheres making up the light ranges. Directional
            lights are ignored for this.

            This is only likely to be useful for multipass additive lighting
            algorithms, where the scene has already been 'seeded' with an ambient
            pass and this pass is just adding light in affected areas.

            When using Ogre::SHADOWTYPE_STENCIL_ADDITIVE or Ogre::SHADOWTYPE_TEXTURE_ADDITIVE,
            this option is implicitly used for all per-light passes and does
            not need to be specified. If you are not using shadows or are using
            a modulative or an integrated shadow technique then this could be useful.

        */
        void setLightScissoringEnabled(bool enabled) { mLightScissoring = enabled; }
        /** Gets whether or not this pass will be clipped by a scissor rectangle
            encompassing the lights that are being used in it.
        */
        bool getLightScissoringEnabled() const { return mLightScissoring; }

        /** Sets whether or not this pass will be clipped by user clips planes
            bounding the area covered by the light.

            This option will only function if there is a single non-directional light being used in
            this pass. If there is more than one light, or only directional lights, then no clipping
            will occur. If there are no lights at all then the objects won’t be rendered at all.

            In order to cut down on the geometry set up to render this pass
            when you have a single fixed-range light being rendered through it,
            you can enable this option to request that during triangle setup,
            clip planes are defined to bound the range of the light. In the case
            of a point light these planes form a cube, and in the case of
            a spotlight they form a pyramid. Directional lights are never clipped.

            This option is only likely to be useful for multipass additive lighting
            algorithms, where the scene has already been 'seeded' with an ambient
            pass and this pass is just adding light in affected areas. In addition,
            it will only be honoured if there is exactly one non-directional light
            being used in this pass. Also, these clip planes override any user clip
            planes set on Camera.

            When using Ogre::SHADOWTYPE_STENCIL_ADDITIVE or Ogre::SHADOWTYPE_TEXTURE_ADDITIVE,
            this option is automatically used for all per-light passes if you
            enable Ogre::SceneManager::setShadowUseLightClipPlanes and does
            not need to be specified. It is disabled by default since clip planes have
            a cost of their own which may not always exceed the benefits they give you.
            Generally the smaller your lights are the more chance you’ll see a benefit rather than
            a penalty from clipping.

            @note A specific note about OpenGL: user clip planes are completely ignored when you use
            an ARB vertex program. This means light clip planes won’t help much if you use ARB
            vertex programs on GL, although OGRE will perform some optimisation of its own, in that
            if it sees that the clip volume is completely off-screen, it won’t perform a render at
            all. When using GLSL, user clipping can be used but you have to use gl_ClipVertex in your
            shader, see the GLSL documentation for more information. In Direct3D user clip planes are
            always respected.
        */
        void setLightClipPlanesEnabled(bool enabled) { mLightClipPlanes = enabled; }
        /** Gets whether or not this pass will be clipped by user clips planes
            bounding the area covered by the light.
        */
        bool getLightClipPlanesEnabled() const { return mLightClipPlanes; }

        /** Manually set which illumination stage this pass is a member of.

            When using an additive lighting mode (Ogre::SHADOWTYPE_STENCIL_ADDITIVE or
            Ogre::SHADOWTYPE_TEXTURE_ADDITIVE), the scene is rendered in 3 discrete
            stages, ambient (or pre-lighting), per-light (once per light, with
            shadowing) and decal (or post-lighting). Usually OGRE figures out how
            to categorise your passes automatically, but there are some effects you
            cannot achieve without manually controlling the illumination. For example
            specular effects are muted by the typical sequence because all textures
            are saved until the Ogre::IS_DECAL stage which mutes the specular effect.
            Instead, you could do texturing within the per-light stage if it's
            possible for your material and thus add the specular on after the
            decal texturing, and have no post-light rendering.

            If you assign an illumination stage to a pass you have to assign it
            to all passes in the technique otherwise it will be ignored. Also note
            that whilst you can have more than one pass in each group, they cannot
            alternate, ie all ambient passes will be before all per-light passes,
            which will also be before all decal passes. Within their categories
            the passes will retain their ordering though.
        */
        void setIlluminationStage(IlluminationStage is) { mIlluminationStage = is; }
        /// Get the manually assigned illumination stage, if any
        IlluminationStage getIlluminationStage() const { return mIlluminationStage; }
        /** There are some default hash functions used to order passes so that
            render state changes are minimised, this enumerates them.
        */
        enum BuiltinHashFunction
        {
            /** Try to minimise the number of texture changes. */
            MIN_TEXTURE_CHANGE,
            /** Try to minimise the number of GPU program changes.
                @note Only really useful if you use GPU programs for all of your
                materials.
            */
            MIN_GPU_PROGRAM_CHANGE
        };
        /** Sets one of the default hash functions to be used.
            @remarks
            You absolutely must not change the hash function whilst any Pass instances
            exist in the render queue. The only time you can do this is either
            before you render anything, or directly after you manuall call
            RenderQueue::clear(true) to completely destroy the queue structures.
            The default is MIN_GPU_PROGRAM_CHANGE.
            @note
            You can also implement your own hash function, see the alternate version
            of this method.
            @see HashFunc
        */
        static void setHashFunction(BuiltinHashFunction builtin);

        /** Set the hash function used for all passes.
            @remarks
            You absolutely must not change the hash function whilst any Pass instances
            exist in the render queue. The only time you can do this is either
            before you render anything, or directly after you manuall call
            RenderQueue::clear(true) to completely destroy the queue structures.
            @note
            You can also use one of the built-in hash functions, see the alternate version
            of this method. The default is MIN_GPU_PROGRAM_CHANGE.
            @see HashFunc
        */
        static void setHashFunction(HashFunc* hashFunc) { msHashFunc = hashFunc; }

        /** Get the hash function used for all passes.
         */
        static HashFunc* getHashFunction(void) { return msHashFunc; }

        /** Get the builtin hash function.
         */
        static HashFunc* getBuiltinHashFunction(BuiltinHashFunction builtin);

        /** Return an instance of user objects binding associated with this class.
            You can use it to associate one or more custom objects with this class instance.
            @see UserObjectBindings::setUserAny.
        */
        UserObjectBindings&     getUserObjectBindings() { return mUserObjectBindings; }

        /** Return an instance of user objects binding associated with this class.
            You can use it to associate one or more custom objects with this class instance.
            @see UserObjectBindings::setUserAny.
        */
        const UserObjectBindings& getUserObjectBindings() const { return mUserObjectBindings; }
     protected:
        std::unique_ptr<GpuProgramUsage>& getProgramUsage(GpuProgramType programType);
        const std::unique_ptr<GpuProgramUsage>& getProgramUsage(GpuProgramType programType) const;
    };

    /** Struct recording a pass which can be used for a specific illumination stage.
        @remarks
        This structure is used to record categorised passes which fit into a
        number of distinct illumination phases - ambient, diffuse / specular
        (per-light) and decal (post-lighting texturing).
        An original pass may fit into one of these categories already, or it
        may require splitting into its component parts in order to be categorised
        properly.
    */
    struct IlluminationPass : public PassAlloc
    {
        IlluminationStage stage;
        /// The pass to use in this stage
        Pass* pass;
        /// Whether this pass is one which should be deleted itself
        bool destroyOnShutdown;
        /// The original pass which spawned this one
        Pass* originalPass;

        IlluminationPass() {}
    };

    typedef std::vector<IlluminationPass*> IlluminationPassList;

    /** @} */
    /** @} */

}

#include "OgreHeaderSuffix.h"

#endif
