#include "stdafx.h"
#include "CustomHeadLayer.h"
#include "LivingEntityRenderer.h"
#include "ModelPart.h"
#include "SkullTileRenderer.h"
#include "TileRenderer.h"
#include "EntityRenderDispatcher.h"
#include "Minecraft.h"
#include "../Minecraft.World/ItemInstance.h"
#include "../Minecraft.World/Item.h"
#include "../Minecraft.World/Tile.h"
#include "../Minecraft.World/SkullItem.h"
#include "../Minecraft.World/SkullTileEntity.h"
#include "../Minecraft.World/LivingEntity.h"
#include "../Minecraft.World/Facing.h"
#include "../Minecraft.Client/SimpleIcon.h"
#include "../Minecraft.World/AirTile.h"


CustomHeadLayer::CustomHeadLayer(ModelPart* headPart, LivingEntityRenderer* parentRenderer)
    : headPart(headPart), parentRenderer(parentRenderer)
{
    tileRenderer = new TileRenderer();

}

CustomHeadLayer::~CustomHeadLayer() {
    delete tileRenderer;
}

int CustomHeadLayer::colorsOnDamage()
{
    return 1;
}

void CustomHeadLayer::render(shared_ptr<LivingEntity> mob,
                              float wp, float ws, float bob,
                              float headRot, float headRotX,
                              float scale, bool useCompiled)
{
    shared_ptr<ItemInstance> helmet = mob->getArmor(3);
    if (!helmet) return;

    Item* item = helmet->getItem();
    if (!item) return;

    if (mob->instanceof(eTYPE_PLAYER))
    {
        _SkinAdjustments adj;
        mob->getSkinAdjustments(&adj);
        if ((adj.data[0] & 0x100) != 0) return;
    }

    bool isBaby  = mob->isBaby();
    // FIX: Riconosciamo correttamente lo SkullItem invece dell'ID del Blocco
    bool isSkull = (dynamic_cast<SkullItem*>(item) != nullptr);

    glPushMatrix();
   
    if (isBaby)
        glTranslatef(0.0f, 0.2f, 0.0f);

    headPart->translateTo(0.0625f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    if (isSkull)
    {
        glScalef(1.1875f, -1.1875f, -1.1875f);

        if (isBaby)
            glTranslatef(0.0f, 0.0625f, 0.0f);

        if (SkullTileRenderer::instance)
        {
            int     skullType = helmet->getAuxValue() & 0xF;
            wstring extra     = L"";
            if (helmet->hasTag() && helmet->getTag()->contains(L"SkullOwner"))
                extra = helmet->getTag()->getString(L"SkullOwner");

            SkullTileRenderer::instance->renderSkull(
                //Skull on armor stand is slightly lowered
                -0.5f, -0.05f, -0.5f,
                Facing::UP,
                180.0f,
                skullType,
                extra
            );
        }
    }
    else if (item->id < 256)
    {
        glTranslatef(0.0f, -0.25f, 0.0f);
        glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
        glScalef(0.625f, -0.625f, -0.625f);

        if (isBaby)
            glTranslatef(0.0f, 0.1875f, 0.0f);

        glRotatef(90.0f, 0.0f, 1.0f, 0.0f);

        {
            // 4J - code borrowed from render method below, although not factoring in brightness as that should already be being taken into account
            // by texture lighting. This is for colourising things held in 3rd person view.
            if ((item != nullptr)) {
                int col = Item::items[item->id]->getColor(helmet, 0);
                float red = ((col >> 16) & 0xff) / 255.0f;
                float g = ((col >> 8) & 0xff) / 255.0f;
                float b = ((col) & 0xff) / 255.0f;

                glColor4f(red, g, b, 1);
            }

            Minecraft* minecraft = Minecraft::GetInstance();

            glPushMatrix();

            Tile* tile = Tile::tiles[item->id];
            if ((item->getIconType() == Icon::TYPE_TERRAIN && tile != nullptr && TileRenderer::canRender(tile->getRenderShape())) && item->id != AirTile::barrier_Id)
            {
                MemSect(31);
                minecraft->textures->bindTexture(minecraft->textures->getTextureLocation(Icon::TYPE_TERRAIN));
                MemSect(0);
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                tileRenderer->renderTile(Tile::tiles[item->id], helmet->getAuxValue(), SharedConstants::TEXTURE_LIGHTING ? 1.0f : mob->getBrightness(1));		// 4J - change brought forward from 1.8.2
                glDisable(GL_BLEND);
            }
            else
            {
                MemSect(31);
                Icon* icon = mob->getItemInHandIcon(helmet, 0);
                if (icon == nullptr)
                {
                    glPopMatrix();
                    MemSect(0);
                    return;
                }

                bool bIsTerrain = item->getIconType() == Icon::TYPE_TERRAIN;
                minecraft->textures->bindTexture(minecraft->textures->getTextureLocation(item->getIconType()));

                MemSect(0);
                Tesselator* t = Tesselator::getInstance();

                // Consider forcing the mipmap LOD level to use, if this is to be rendered from a larger than standard source texture. 
                int iconWidth = icon->getWidth();
                int LOD = -1;	// Default to not doing anything special with LOD forcing
                if (iconWidth == 32)
                {
                    LOD = 1;	// Force LOD level 1 to achieve texture reads from 256x256 map
                }
                else if (iconWidth == 64)
                {
                    LOD = 2;	// Force LOD level 2 to achieve texture reads from 256x256 map
                }
                RenderManager.StateSetForceLOD(LOD);

                // 4J Original comment
                // Yes, these are backwards.
                // No, I don't know why.
                // 4J Stu - Make them the right way round...u coords were swapped
                float u0 = icon->getU0();
                float u1 = icon->getU1();
                float v0 = icon->getV0();
                float v1 = icon->getV1();

                float xo = 0.0f;
                float yo = 0.3f;

                // Re position height of held item if skin is small
                if (mob->getAnimOverrideBitmask() & (1 << HumanoidModel::eAnim_SmallModel))
                {
                    if (mob->isRiding())
                    {
                        std::shared_ptr<Entity> ridingEntity = mob->riding;
                        if (ridingEntity != nullptr) // Safety check;
                        {
                            yo += 0.3f; // reverts the change in Boat.cpp for smaller models.
                        }
                    }
                }

                glEnable(GL_RESCALE_NORMAL);
                glTranslatef(-xo, -yo, 0);
                glScalef(1, 1, 1);

                glRotatef(90, 0, 1, 0);
                glTranslatef(-7.5f / 16.0f, 9 / 16.0f, 0);
                glTranslatef(0, 0.03f, 0);
                float dd = 1 / 16.0f;

                ItemInHandRenderer::renderItem3D(t, u0, v0, u1, v1, icon->getSourceWidth(), icon->getSourceHeight(), 1 / 16.0f, false, bIsTerrain);

                if (item != nullptr && helmet->isFoil())
                {
                    glDepthFunc(GL_EQUAL);
                    glDisable(GL_LIGHTING);
                    minecraft->textures->bindTexture(&ItemInHandRenderer::ENCHANT_GLINT_LOCATION);
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_COLOR, GL_ONE);
                    float br = 0.76f;
                    glColor4f(0.5f * br, 0.25f * br, 0.8f * br, 1);		// MGH - for some reason this colour isn't making it through to the render, so I've added to the tesselator for the glint geom above
                    glMatrixMode(GL_TEXTURE);
                    glPushMatrix();
                    float ss = 1 / 8.0f;
                    glScalef(ss, ss, ss);
                    float sx = Minecraft::currentTimeMillis() % (3000) / (3000.0f) * 8;
                    glTranslatef(sx, 0, 0);
                    glRotatef(-50, 0, 0, 1);

                    ItemInHandRenderer::renderItem3D(t, 0, 0, 1, 1, 256, 256, 1 / 16.0f, true, bIsTerrain);
                    glPopMatrix();
                    glPushMatrix();
                    glScalef(ss, ss, ss);
                    sx = System::currentTimeMillis() % (3000 + 1873) / (3000 + 1873.0f) * 8;
                    glTranslatef(-sx, 0, 0);
                    glRotatef(10, 0, 0, 1);
                    ItemInHandRenderer::renderItem3D(t, 0, 0, 1, 1, 256, 256, 1 / 16.0f, true, bIsTerrain);
                    glPopMatrix();
                    glMatrixMode(GL_MODELVIEW);
                    glDisable(GL_BLEND);
                    glEnable(GL_LIGHTING);
                    glDepthFunc(GL_LEQUAL);
                }

                RenderManager.StateSetForceLOD(-1);

                glDisable(GL_RESCALE_NORMAL);
            }
            glPopMatrix();
        }
    }

    glPopMatrix();
}