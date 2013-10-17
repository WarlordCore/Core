
/*
 * Copyright (C) 2008-2013 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Scripts for spells with SPELLFAMILY_PALADIN and SPELLFAMILY_GENERIC spells used by paladin players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pal_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "Group.h"

enum PaladinSpells
{
    SPELL_PALADIN_DIVINE_PLEA                    = 54428,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF     = 67480,
    SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE = 57319,

    SPELL_PALADIN_HOLY_SHOCK_R1                  = 20473,
    SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE           = 25912,
    SPELL_PALADIN_HOLY_SHOCK_R1_HEALING          = 25914,

    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID   = 37878,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN = 37879,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST  = 37880,
    SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN  = 37881,

    SPELL_PALADIN_DIVINE_STORM                   = 53385,
    SPELL_PALADIN_DIVINE_STORM_DUMMY             = 54171,
    SPELL_PALADIN_DIVINE_STORM_HEAL              = 54172,

	SPELL_PALADIN_AVENGER_S_SHIELD               = 31935,

    SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE          = 25997,

    SPELL_PALADIN_FORBEARANCE                    = 25771,
    SPELL_PALADIN_AVENGING_WRATH_MARKER          = 61987,
    SPELL_PALADIN_IMMUNE_SHIELD_MARKER           = 61988,

    SPELL_PALADIN_HAND_OF_SACRIFICE              = 6940,
    SPELL_PALADIN_DIVINE_SACRIFICE               = 64205,

    SPELL_PALADIN_GLYPH_OF_SALVATION             = 63225,

    SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT        = 31790,

    SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS          = 25742,

    SPELL_PALADIN_DIVINE_PURPOSE_PROC            = 90174,
    SPELL_PALADIN_WORD_OF_GLORY                  = 85673,
    SPELL_PALADIN_JUDG_BOLD_OVERTIME             = 89906,
    SPELL_PALADIN_SELFLESS_HEALER_PROC           = 90811,

    SPELL_PALADIN_RETRI_GUARDIAN                 = 86698,
    SPELL_PALADIN_HOLY_GUARDIAN                  = 86669,
    SPELL_PALADIN_PROT_GUARDIAN                  = 86659,

    SPELL_PALADIN_CONSECRATION_SUMMON            = 82366,
    SPELL_PALADIN_CONSECRATION_DAMAGE            = 81297,

    SPELL_GENERIC_ARENA_DAMPENING                = 74410,
    SPELL_GENERIC_BATTLEGROUND_DAMPENING         = 74411,
    SPELL_PALADIN_SPEED_OF_LIGHT_BUFF            = 85497
};

// 31850 - Ardent Defender
/*class spell_pal_ardent_defender : public SpellScriptLoader
{
    public:
        spell_pal_ardent_defender() : SpellScriptLoader("spell_pal_ardent_defender") { }

        class spell_pal_ardent_defender_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_ardent_defender_AuraScript);

            uint32 absorbPct, healPct;

            enum Spell
            {
                PAL_SPELL_ARDENT_DEFENDER_HEAL = 66235,
            };

            bool Load()
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                absorbPct = GetSpellInfo()->Effects[EFFECT_0].CalcValue();
                return GetUnitOwner()->GetTypeId() == TYPEID_PLAYER;
            }

            void CalculateAmount(AuraEffect const* aurEff, int32 & amount, bool & canBeRecalculated)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* aurEff, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* victim = GetTarget();
                int32 remainingHealth = victim->GetHealth() - dmgInfo.GetDamage();
                uint32 allowedHealth = victim->CountPctFromMaxHealth(35);
                // If damage kills us
                if (remainingHealth <= 0 && !victim->ToPlayer()->HasSpellCooldown(PAL_SPELL_ARDENT_DEFENDER_HEAL))
                {
                    // Cast healing spell, completely avoid damage
                    absorbAmount = dmgInfo.GetDamage();

                    uint32 defenseSkillValue = victim->GetDefenseSkillValue();
                    // Max heal when defense skill denies critical hits from raid bosses
                    // Formula: max defense at level + 140 (raiting from gear)
                    uint32 reqDefForMaxHeal  = victim->getLevel() * 5 + 140;
                    float pctFromDefense = (defenseSkillValue >= reqDefForMaxHeal)
                        ? 1.0f
                        : float(defenseSkillValue) / float(reqDefForMaxHeal);

                    int32 healAmount = int32(victim->CountPctFromMaxHealth(uint32(healPct * pctFromDefense)));
                    victim->CastCustomSpell(victim, PAL_SPELL_ARDENT_DEFENDER_HEAL, &healAmount, NULL, NULL, true, NULL, aurEff);
                    victim->ToPlayer()->AddSpellCooldown(PAL_SPELL_ARDENT_DEFENDER_HEAL, 0, time(NULL) + 120);
                }
                else if (remainingHealth < int32(allowedHealth))
                {
                    // Reduce damage that brings us under 35% (or full damage if we are already under 35%) by x%
                    uint32 damageToReduce = (victim->GetHealth() < allowedHealth)
                        ? dmgInfo.GetDamage()
                        : allowedHealth - remainingHealth;
                    absorbAmount = CalculatePct(damageToReduce, absorbPct);
                }
            }

            void Register()
            {
                 DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_ardent_defender_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                 OnEffectAbsorb += AuraEffectAbsorbFn(spell_pal_ardent_defender_AuraScript::Absorb, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_ardent_defender_AuraScript();
        }
};*/

// 37877 - Blessing of Faith
class spell_pal_blessing_of_faith : public SpellScriptLoader
{
    public:
        spell_pal_blessing_of_faith() : SpellScriptLoader("spell_pal_blessing_of_faith") { }

        class spell_pal_blessing_of_faith_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_blessing_of_faith_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID) || !sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN) || !sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST) || !sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN))
                    return false;
                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                {
                    uint32 spell_id = 0;
                    switch (unitTarget->getClass())
                    {
                        case CLASS_DRUID:
                            spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_DRUID;
                            break;
                        case CLASS_PALADIN:
                            spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PALADIN;
                            break;
                        case CLASS_PRIEST:
                            spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_PRIEST;
                            break;
                        case CLASS_SHAMAN:
                            spell_id = SPELL_PALADIN_BLESSING_OF_LOWER_CITY_SHAMAN;
                            break;
                        default:
                            return; // ignore for non-healing classes
                    }
                    Unit* caster = GetCaster();
                    caster->CastSpell(caster, spell_id, true);
                }
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_blessing_of_faith_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_blessing_of_faith_SpellScript();
        }
};

// 20911 - Blessing of Sanctuary
// 25899 - Greater Blessing of Sanctuary
class spell_pal_blessing_of_sanctuary : public SpellScriptLoader
{
    public:
        spell_pal_blessing_of_sanctuary() : SpellScriptLoader("spell_pal_blessing_of_sanctuary") { }

        class spell_pal_blessing_of_sanctuary_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_blessing_of_sanctuary_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE))
                    return false;
                return true;
            }

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                if (Unit* caster = GetCaster())
                    caster->CastSpell(target, SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF, true);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                Unit* target = GetTarget();
                target->RemoveAura(SPELL_PALADIN_BLESSING_OF_SANCTUARY_BUFF, GetCasterGUID());
            }

            bool CheckProc(ProcEventInfo& /*eventInfo*/)
            {
                return GetTarget()->getPowerType() == POWER_MANA;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(GetTarget(), SPELL_PALADIN_BLESSING_OF_SANCTUARY_ENERGIZE, true, NULL, aurEff);
            }

            void Register()
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pal_blessing_of_sanctuary_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pal_blessing_of_sanctuary_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                DoCheckProc += AuraCheckProcFn(spell_pal_blessing_of_sanctuary_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_blessing_of_sanctuary_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_blessing_of_sanctuary_AuraScript();
        }
};

// 64205 - Divine Sacrifice
class spell_pal_divine_sacrifice : public SpellScriptLoader
{
    public:
        spell_pal_divine_sacrifice() : SpellScriptLoader("spell_pal_divine_sacrifice") { }

        class spell_pal_divine_sacrifice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_divine_sacrifice_AuraScript);

            uint32 groupSize, minHpPct;
            int32 remainingAmount;

            bool Load()
            {

                if (Unit* caster = GetCaster())
                {
                    if (caster->GetTypeId() == TYPEID_PLAYER)
                    {
                        if (caster->ToPlayer()->GetGroup())
                            groupSize = caster->ToPlayer()->GetGroup()->GetMembersCount();
                        else
                            groupSize = 1;
                    }
                    else
                        return false;

                    remainingAmount = (caster->CountPctFromMaxHealth(GetSpellInfo()->Effects[EFFECT_2].CalcValue(caster)) * groupSize);
                    minHpPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(caster);
                    return true;
                }
                return false;
            }

            void Split(AuraEffect* /*aurEff*/, DamageInfo & /*dmgInfo*/, uint32 & splitAmount)
            {
                remainingAmount -= splitAmount;
                // break when absorbed everything it could, or if the casters hp drops below 20%
                if (Unit* caster = GetCaster())
                    if (remainingAmount <= 0 || (caster->GetHealthPct() < minHpPct))
                        caster->RemoveAura(SPELL_PALADIN_DIVINE_SACRIFICE);
            }

            void Register()
            {
                OnEffectSplit += AuraEffectSplitFn(spell_pal_divine_sacrifice_AuraScript::Split, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_divine_sacrifice_AuraScript();
        }
};

// 53385 - Divine Storm
class spell_pal_divine_storm : public SpellScriptLoader
{
    public:
        spell_pal_divine_storm() : SpellScriptLoader("spell_pal_divine_storm") { }

        class spell_pal_divine_storm_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_storm_SpellScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_DIVINE_STORM_DUMMY))
                    return false;
                return true;
            }

            bool Load()
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue(GetCaster());
                return true;
            }

            void TriggerHeal()
            {
                Unit* caster = GetCaster();
                caster->CastCustomSpell(SPELL_PALADIN_DIVINE_STORM_DUMMY, SPELLVALUE_BASE_POINT0, (GetHitDamage() * healPct) / 100, caster, true);
            }

            void Register()
            {
                AfterHit += SpellHitFn(spell_pal_divine_storm_SpellScript::TriggerHeal);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_divine_storm_SpellScript();
        }
};

// 54171 - Divine Storm (Dummy)
class spell_pal_divine_storm_dummy : public SpellScriptLoader
{
    public:
        spell_pal_divine_storm_dummy() : SpellScriptLoader("spell_pal_divine_storm_dummy") { }

        class spell_pal_divine_storm_dummy_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_divine_storm_dummy_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_DIVINE_STORM_HEAL))
                    return false;
                return true;
            }

            void CountTargets(std::list<WorldObject*>& targetList)
            {
                _targetCount = targetList.size();
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (!_targetCount || ! GetHitUnit())
                    return;

                int32 heal = GetEffectValue() / _targetCount;
                GetCaster()->CastCustomSpell(GetHitUnit(), SPELL_PALADIN_DIVINE_STORM_HEAL, &heal, NULL, NULL, true);
            }
        private:
            uint32 _targetCount;

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_divine_storm_dummy_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_divine_storm_dummy_SpellScript::CountTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_RAID);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_divine_storm_dummy_SpellScript();
        }
};

// 33695 - Exorcism and Holy Wrath Damage
class spell_pal_exorcism_and_holy_wrath_damage : public SpellScriptLoader
{
    public:
        spell_pal_exorcism_and_holy_wrath_damage() : SpellScriptLoader("spell_pal_exorcism_and_holy_wrath_damage") { }

        class spell_pal_exorcism_and_holy_wrath_damage_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_exorcism_and_holy_wrath_damage_AuraScript);

            void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
            {
                if (!spellMod)
                {
                    spellMod = new SpellModifier(aurEff->GetBase());
                    spellMod->op = SPELLMOD_DAMAGE;
                    spellMod->type = SPELLMOD_FLAT;
                    spellMod->spellId = GetId();
                    spellMod->mask[1] = 0x200002;
                }

                spellMod->value = aurEff->GetAmount();
            }

            void Register()
            {
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pal_exorcism_and_holy_wrath_damage_AuraScript::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_exorcism_and_holy_wrath_damage_AuraScript();
        }
};

// -9799 - Eye for an Eye
class spell_pal_eye_for_an_eye : public SpellScriptLoader
{
    public:
        spell_pal_eye_for_an_eye() : SpellScriptLoader("spell_pal_eye_for_an_eye") { }

        class spell_pal_eye_for_an_eye_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_eye_for_an_eye_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE))
                    return false;
                return true;
            }

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                // return damage % to attacker but < 50% own total health
                int32 damage = int32(std::min(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()), GetTarget()->GetMaxHealth() / 2));
                GetTarget()->CastCustomSpell(SPELL_PALADIN_EYE_FOR_AN_EYE_DAMAGE, SPELLVALUE_BASE_POINT0, damage, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_eye_for_an_eye_AuraScript::OnProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_eye_for_an_eye_AuraScript();
        }
};

// 75806, 85043, 85416 - Grand Crusader
/// Updated 4.3.4
class spell_pal_grand_crusader : public SpellScriptLoader
{
    public:
        spell_pal_grand_crusader() : SpellScriptLoader("spell_pal_grand_crusader") { }

        class spell_pal_grand_crusader_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_grand_crusader_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_AVENGER_S_SHIELD))
                    return false;
                return true;
            }

            void HandleApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                    caster->ToPlayer()->RemoveSpellCooldown(SPELL_PALADIN_AVENGER_S_SHIELD, true);
            }

            void Register()
            {
                OnEffectApply += AuraEffectApplyFn(spell_pal_grand_crusader_AuraScript::HandleApply, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_grand_crusader_AuraScript();
        }
};

// 63521 - Guarded by The Light
class spell_pal_guarded_by_the_light : public SpellScriptLoader
{
    public:
        spell_pal_guarded_by_the_light() : SpellScriptLoader("spell_pal_guarded_by_the_light") { }

        class spell_pal_guarded_by_the_light_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_guarded_by_the_light_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_DIVINE_PLEA))
                    return false;
                return true;
            }

            void HandleScriptEffect(SpellEffIndex /*effIndex*/)
            {
                // Divine Plea
                if (Aura* aura = GetCaster()->GetAura(SPELL_PALADIN_DIVINE_PLEA))
                    aura->RefreshDuration();
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_guarded_by_the_light_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_guarded_by_the_light_SpellScript();
        }
};

// 6940 - Hand of Sacrifice
class spell_pal_hand_of_sacrifice : public SpellScriptLoader
{
    public:
        spell_pal_hand_of_sacrifice() : SpellScriptLoader("spell_pal_hand_of_sacrifice") { }

        class spell_pal_hand_of_sacrifice_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_hand_of_sacrifice_AuraScript);

            int32 remainingAmount;

            bool Load()
            {
                if (Unit* caster = GetCaster())
                {
                    remainingAmount = caster->GetMaxHealth();
                    return true;
                }
                return false;
            }

            void Split(AuraEffect* /*aurEff*/, DamageInfo & /*dmgInfo*/, uint32 & splitAmount)
            {
                remainingAmount -= splitAmount;

                if (remainingAmount <= 0)
                {
                    GetTarget()->RemoveAura(SPELL_PALADIN_HAND_OF_SACRIFICE);
                }
            }

            void Register()
            {
                OnEffectSplit += AuraEffectSplitFn(spell_pal_hand_of_sacrifice_AuraScript::Split, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_hand_of_sacrifice_AuraScript();
        }
};

// 1038 - Hand of Salvation
class spell_pal_hand_of_salvation : public SpellScriptLoader
{
    public:
        spell_pal_hand_of_salvation() : SpellScriptLoader("spell_pal_hand_of_salvation") { }

        class spell_pal_hand_of_salvation_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_hand_of_salvation_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    // Glyph of Salvation
                    if (caster->GetGUID() == GetUnitOwner()->GetGUID())
                        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_PALADIN_GLYPH_OF_SALVATION, EFFECT_0))
                            amount -= aurEff->GetAmount();
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_hand_of_salvation_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_MOD_DAMAGE_PERCENT_TAKEN);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_hand_of_salvation_AuraScript();
        }
};

// -20473 - Holy Shock
class spell_pal_holy_shock : public SpellScriptLoader
{
    public:
        spell_pal_holy_shock() : SpellScriptLoader("spell_pal_holy_shock") { }

        class spell_pal_holy_shock_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_holy_shock_SpellScript);

            bool Validate(SpellInfo const* spell)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_HOLY_SHOCK_R1))
                    return false;

                // can't use other spell than holy shock due to spell_ranks dependency
                if (sSpellMgr->GetFirstSpellInChain(SPELL_PALADIN_HOLY_SHOCK_R1) != sSpellMgr->GetFirstSpellInChain(spell->Id))
                    return false;

                uint8 rank = sSpellMgr->GetSpellRank(spell->Id);
                if (!sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank, true) || !sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank, true))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    uint8 rank = sSpellMgr->GetSpellRank(GetSpellInfo()->Id);
                    if (caster->IsFriendlyTo(unitTarget))
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_HEALING, rank), true, 0);
                    else
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PALADIN_HOLY_SHOCK_R1_DAMAGE, rank), true, 0);
                }
            }

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetExplTargetUnit())
                {
                    if (!caster->IsFriendlyTo(target))
                    {
                        if (!caster->IsValidAttackTarget(target))
                            return SPELL_FAILED_BAD_TARGETS;

                        if (!caster->isInFront(target))
                            return SPELL_FAILED_UNIT_NOT_INFRONT;
                    }
                }
                else
                    return SPELL_FAILED_BAD_TARGETS;
                return SPELL_CAST_OK;
            }

        void CalculateDamage (SpellEffIndex /*effIndex*/)
       {
		Unit* caster = GetCaster();

	    if (Unit* target = GetExplTargetUnit())
           {
            	if (!caster->IsFriendlyTo(target))
           	 {
                	int32 damage = GetHitDamage();
             	       if (!damage)
                  	  damage = 13 * caster->getLevel();
  
                  	damage += caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.16;

                	SetHitDamage(damage);
            		}
        	 }
            }
            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_holy_shock_SpellScript::CheckCast);
                OnEffectHitTarget += SpellEffectFn(spell_pal_holy_shock_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
               OnEffectHitTarget += SpellEffectFn(spell_pal_holy_shock_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_holy_shock_SpellScript();
        }
};

// 20425 - Judgement of Command
class spell_pal_judgement_of_command : public SpellScriptLoader
{
    public:
        spell_pal_judgement_of_command() : SpellScriptLoader("spell_pal_judgement_of_command") { }

        class spell_pal_judgement_of_command_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_judgement_of_command_SpellScript);

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                if (Unit* unitTarget = GetHitUnit())
                    if (SpellInfo const* spell_proto = sSpellMgr->GetSpellInfo(GetEffectValue()))
                        GetCaster()->CastSpell(unitTarget, spell_proto, true, NULL);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_judgement_of_command_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_judgement_of_command_SpellScript();
        }
};

// -633 - Lay on Hands
class spell_pal_lay_on_hands : public SpellScriptLoader
{
    public:
        spell_pal_lay_on_hands() : SpellScriptLoader("spell_pal_lay_on_hands") { }

        class spell_pal_lay_on_hands_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_lay_on_hands_SpellScript);

            bool Validate(SpellInfo const* /*spell*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_FORBEARANCE))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_AVENGING_WRATH_MARKER))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_IMMUNE_SHIELD_MARKER))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (Unit* target = GetExplTargetUnit())
                    if (caster == target)
                        if (target->HasAura(SPELL_PALADIN_FORBEARANCE) || target->HasAura(SPELL_PALADIN_AVENGING_WRATH_MARKER) || target->HasAura(SPELL_PALADIN_IMMUNE_SHIELD_MARKER))
                            return SPELL_FAILED_TARGET_AURASTATE;

                return SPELL_CAST_OK;
            }

            void HandleScript()
            {
                Unit* caster = GetCaster();
                if (caster == GetHitUnit())
                {
                    caster->CastSpell(caster, SPELL_PALADIN_FORBEARANCE, true);
                    caster->CastSpell(caster, SPELL_PALADIN_AVENGING_WRATH_MARKER, true);
                    caster->CastSpell(caster, SPELL_PALADIN_IMMUNE_SHIELD_MARKER, true);
                }
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_lay_on_hands_SpellScript::CheckCast);
                AfterHit += SpellHitFn(spell_pal_lay_on_hands_SpellScript::HandleScript);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_lay_on_hands_SpellScript();
        }
};

// 31789 - Righteous Defense
class spell_pal_righteous_defense : public SpellScriptLoader
{
    public:
        spell_pal_righteous_defense() : SpellScriptLoader("spell_pal_righteous_defense") { }

        class spell_pal_righteous_defense_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_righteous_defense_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT))
                    return false;
                return true;
            }

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_DONT_REPORT;

                if (Unit* target = GetExplTargetUnit())
                {
                    if (!target->IsFriendlyTo(caster) || target->getAttackers().empty())
                        return SPELL_FAILED_BAD_TARGETS;
                }
                else
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }

            void HandleTriggerSpellLaunch(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
            }

            void HandleTriggerSpellHit(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                if (Unit* target = GetHitUnit())
                    GetCaster()->CastSpell(target, SPELL_PALADIN_RIGHTEOUS_DEFENSE_TAUNT, true);
            }

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_righteous_defense_SpellScript::CheckCast);
                //! WORKAROUND
                //! target select will be executed in hitphase of effect 0
                //! so we must handle trigger spell also in hit phase (default execution in launch phase)
                //! see issue #3718
                OnEffectLaunchTarget += SpellEffectFn(spell_pal_righteous_defense_SpellScript::HandleTriggerSpellLaunch, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
                OnEffectHitTarget += SpellEffectFn(spell_pal_righteous_defense_SpellScript::HandleTriggerSpellHit, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_righteous_defense_SpellScript();
        }
};

// 85285 - Sacred Shield
class spell_pal_sacred_shield : public SpellScriptLoader
{
    public:
        spell_pal_sacred_shield() : SpellScriptLoader("spell_pal_sacred_shield") { }

        class spell_pal_sacred_shield_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_sacred_shield_SpellScript);

            SpellCastResult CheckCast()
            {
                Unit* caster = GetCaster();
                if (caster->GetTypeId() != TYPEID_PLAYER)
                    return SPELL_FAILED_BAD_TARGETS;

                if (!caster->HealthBelowPct(30))
                    return SPELL_FAILED_BAD_TARGETS;

                return SPELL_CAST_OK;
            }
	     

            void Register()
            {
                OnCheckCast += SpellCheckCastFn(spell_pal_sacred_shield_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_sacred_shield_SpellScript();
        }
};

// 53600 - Shield of the Righteous
/// Updated 4.3.4
class spell_pal_shield_of_the_righteous : public SpellScriptLoader
{
    public:
        spell_pal_shield_of_the_righteous() : SpellScriptLoader("spell_pal_shield_of_the_righteous") { }

        class spell_pal_shield_of_the_righteous_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_shield_of_the_righteous_SpellScript);

            bool Load()
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return false;

                if (GetCaster()->ToPlayer()->getClass() != CLASS_PALADIN)
                    return false;

                return true;
            }

            void ChangeDamage(SpellEffIndex /*effIndex*/)
            {
                int32 damage = GetHitDamage();

                // Because 1 Holy Power (HP) is consumed when casting spell,
                // GetPower(POWER_HOLY_POWER) will return 0 when player has 1 HP,
                // return 1 at 2 HP, and 2 at 3 HP
                int32 hp = GetCaster()->GetPower(POWER_HOLY_POWER);

                // Holy Power Scaling: 3 times damage at 2 HP, 6 times at 3 HP
                damage *= 0.5*hp*hp + 1.5*hp + 1;

                SetHitDamage(damage);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_shield_of_the_righteous_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_shield_of_the_righteous_SpellScript();
        }
};


// 85256 - Templar's Verdict
/// Updated 4.3.4
class spell_pal_templar_s_verdict : public SpellScriptLoader
{
    public:
        spell_pal_templar_s_verdict() : SpellScriptLoader("spell_pal_templar_s_verdict") { }

        class spell_pal_templar_s_verdict_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_templar_s_verdict_SpellScript);

            bool Validate (SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_DIVINE_PURPOSE_PROC))
                    return false;

                return true;
            }

            bool Load()
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return false;

                if (GetCaster()->ToPlayer()->getClass() != CLASS_PALADIN)
                    return false;

                return true;
            }

            void ChangeDamage(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                int32 damage = GetHitDamage();

                if (caster->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_PROC))
                    damage *= 7.5;  // 7.5*30% = 225%
                else
                {
                    switch (caster->GetPower(POWER_HOLY_POWER))
                    {
                        case 0: // 1 Holy Power
                            damage = damage;
                            break;
                        case 1: // 2 Holy Power
                            damage *= 3;    // 3*30 = 90%
                            break;
                        case 2: // 3 Holy Power
                            damage *= 7.5;  // 7.5*30% = 225%
                            break;
                    }
                }

                SetHitDamage(damage);
            }

            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_templar_s_verdict_SpellScript::ChangeDamage, EFFECT_0, SPELL_EFFECT_WEAPON_PERCENT_DAMAGE);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_templar_s_verdict_SpellScript();
        }
};

// 20154, 21084 - Seal of Righteousness - melee proc dummy (addition ${$MWS*(0.022*$AP+0.044*$SPH)} damage)
class spell_pal_seal_of_righteousness : public SpellScriptLoader
{
    public:
        spell_pal_seal_of_righteousness() : SpellScriptLoader("spell_pal_seal_of_righteousness") { }

        class spell_pal_seal_of_righteousness_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_seal_of_righteousness_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                return eventInfo.GetProcTarget();
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                float ap = GetTarget()->GetTotalAttackPowerValue(BASE_ATTACK);
                int32 holy = GetTarget()->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_HOLY);
                holy += eventInfo.GetProcTarget()->SpellBaseDamageBonusTaken(SPELL_SCHOOL_MASK_HOLY);
                int32 bp = int32((ap * 0.022f + 0.044f * holy) * GetTarget()->GetAttackTime(BASE_ATTACK) / 1000);
                GetTarget()->CastCustomSpell(SPELL_PALADIN_SEAL_OF_RIGHTEOUSNESS, SPELLVALUE_BASE_POINT0, bp, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register()
            {
                DoCheckProc += AuraCheckProcFn(spell_pal_seal_of_righteousness_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pal_seal_of_righteousness_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_seal_of_righteousness_AuraScript();
        }
};

class spell_pal_word_of_glory : public SpellScriptLoader
{
public:
    spell_pal_word_of_glory() : SpellScriptLoader("spell_pal_word_of_glory") { }

    class spell_pal_word_of_glory_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_word_of_glory_heal_SpellScript)

        int32 totalheal;

        bool Validate (SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_WORD_OF_GLORY))
                return false;
            return true;
        }

        bool Load()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return false;

            if (GetCaster()->ToPlayer()->getClass() != CLASS_PALADIN)
                    return false;

            return true;
        }

        void ChangeHeal(SpellEffIndex /*effIndex*/)
        {
            totalheal = GetHitHeal();
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();

            if (!target)
                return;

            if (GetCaster()->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_PROC))
            {
                totalheal *= 3;

                // Selfless Healer
                if (AuraEffect const* auraEff = caster->GetAuraEffect(SPELL_AURA_DUMMY, SPELLFAMILY_PALADIN, 3924, EFFECT_0))
                    if (target != caster)
                        totalheal += (totalheal * auraEff->GetAmount()) / 100;

                SetHitHeal(totalheal);
                const_cast<SpellValue*>(GetSpellValue())->EffectBasePoints[1] = totalheal;
                return;
            }

            switch (caster->GetPower(POWER_HOLY_POWER))
            {
                case 0: // 1 Holy Power
                {
                    totalheal = totalheal;
                    break;
                }
                case 1: // 2 Holy Power
                {
                    totalheal *= 2;
                    break;
                }
                case 2: // 3 Holy Power
                {
                    totalheal *= 3;
                    break;
                }
            }
            // Selfless Healer
            if (AuraEffect const* auraEff = caster->GetDummyAuraEffect(SPELLFAMILY_PALADIN, 3924, EFFECT_0))
                if (target != caster)
                    totalheal += (totalheal * auraEff->GetAmount()) / 100;

            SetHitHeal(totalheal);
            const_cast<SpellValue*>(GetSpellValue())->EffectBasePoints[1] = totalheal;
        }

        void PreventEffect(SpellEffIndex effIndex)
        {
            // Glyph of Long Word
            if (!GetCaster()->HasAura(93466))
                PreventHitDefaultEffect(effIndex);
        }

        void HandlePeriodic()
        {
            // Glyph of Long Word
            if (!GetCaster()->HasAura(93466))
                PreventHitAura();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_word_of_glory_heal_SpellScript::ChangeHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            OnEffectHitTarget += SpellEffectFn(spell_pal_word_of_glory_heal_SpellScript::PreventEffect, EFFECT_1, SPELL_EFFECT_APPLY_AURA);
            AfterHit += SpellHitFn(spell_pal_word_of_glory_heal_SpellScript::HandlePeriodic);
        }
    };

    class spell_pal_word_of_glory_heal_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_word_of_glory_heal_AuraScript)

        bool Validate (SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_WORD_OF_GLORY)->Effects[1].Effect)
                return false;
            return true;
        }

        bool Load()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return false;

            if (GetCaster()->ToPlayer()->getClass() != CLASS_PALADIN)
                return false;

            return true;
        }

        void CalculateOvertime(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
        {
            // Had to do ALLLLLL the scaling manually because (afaik) there is no way to hook the GetHitHeal from the spell's effIndex 0
            Unit* caster = GetCaster();
            int32 bonus = (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.198) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.209);
            int32 divinebonus = (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.594) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.627);
            int32 multiplier;

            if (AuraEffect const* longWord = GetCaster()->GetDummyAuraEffect(SPELLFAMILY_PALADIN, 4127, 1))
            {
                canBeRecalculated = true;

                switch (caster->GetPower(POWER_HOLY_POWER))
                {
                    case 0:
                    {
                        bonus = bonus;
                        multiplier = 1;
                        break;
                    }
                    case 1:
                    {
                       bonus = (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.396) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.418);
                       multiplier = 2;
                       break;
                    }
                    case 2:
                    {
                       bonus = (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.594) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.627);
                       multiplier = 3;
                       break;
                    }
                }
                if (caster->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_PROC))
                {
                    amount = ((((GetSpellInfo()->Effects[0].CalcValue(caster) * 3)  + divinebonus) * longWord->GetAmount()) / 100)  / 3;
                }
                else
                    amount = ((((GetSpellInfo()->Effects[0].CalcValue(caster) * multiplier)  + bonus) * longWord->GetAmount()) / 100)  / 3;
            }
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_word_of_glory_heal_AuraScript::CalculateOvertime, EFFECT_1, SPELL_AURA_PERIODIC_HEAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_word_of_glory_heal_AuraScript();
    }
    SpellScript* GetSpellScript() const
    {
        return new spell_pal_word_of_glory_heal_SpellScript();
    }
};

class spell_pal_selfless_healer : public SpellScriptLoader
{
    public:
        spell_pal_selfless_healer() : SpellScriptLoader("spell_pal_selfless_healer") { }

        class spell_pal_selfless_healer_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_selfless_healer_AuraScript);

            bool Load()
            {
                if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                    return false;

                if (GetCaster()->ToPlayer()->getClass() != CLASS_PALADIN)
                    return false;

                return true;
            }

            bool Validate (SpellInfo const* /*spellEntry*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_SELFLESS_HEALER_PROC))
                    return false;
                return true;
            }

            void CalculateBonus(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
            {
                if (Unit* caster = GetCaster())
                {
                    canBeRecalculated = true;

                    if (caster->HasAura(SPELL_PALADIN_DIVINE_PURPOSE_PROC))
                    {
                        amount = 12;
                        return;
                    }

                    switch (caster->GetPower(POWER_HOLY_POWER))
                    {
                        case 0: // 1 Holy Power
                        {
                            amount = 4;
                            break;
                        }
                        case 1: // 2 Holy Power
                        {
                            amount = 8;
                            break;
                        }
                        case 2: // 3 Holy Power
                        {
                            amount = 12;
                            break;
                        }
                    }
                }
            }

            void Register()
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pal_selfless_healer_AuraScript::CalculateBonus, EFFECT_0, SPELL_AURA_MOD_DAMAGE_PERCENT_DONE);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_selfless_healer_AuraScript();
        }
};

class spell_pal_holy_wrath : public SpellScriptLoader
{
public:
        spell_pal_holy_wrath() : SpellScriptLoader("spell_pal_holy_wrath") { }
        
        class spell_pal_holy_wrath_SpellScript : public SpellScript
        {
                PrepareSpellScript(spell_pal_holy_wrath_SpellScript);
                
                void FilterTargets(std::list<WorldObject*>& unitList)
                {
                        std::list<WorldObject*> tempTargets;
                        Unit* caster = GetCaster();
                        if(!caster)
                                return;
                        
                        for (std::list<WorldObject*>::iterator itr = unitList.begin() ; itr != unitList.end(); ++itr)
                        {                       
                                uint32 typeCreature = (*itr)->ToUnit()->GetCreatureType();
                                bool isElemsAndDragonkins = caster->HasAura(2812);  // Stun holy wrathu
                                if ( (typeCreature != CREATURE_TYPE_DEMON || typeCreature != CREATURE_TYPE_UNDEAD) ||
                                        (isElemsAndDragonkins && (typeCreature != CREATURE_TYPE_DRAGONKIN || typeCreature != CREATURE_TYPE_ELEMENTAL)) ) // Overenie creatury 
                                        continue;
                                
                                tempTargets.push_back(*itr);
                        }
                        
                        unitList.clear();
                        for (std::list<WorldObject*>::iterator itr = tempTargets.begin() ; itr != tempTargets.end(); ++itr)
                                unitList.push_back(*itr);
                        
                }
                
                void Register()
                {
                        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pal_holy_wrath_SpellScript::FilterTargets, EFFECT_1, TARGET_SRC_CASTER);
                }
        };
        
        SpellScript* GetSpellScript() const
        {
                return new spell_pal_holy_wrath_SpellScript();
        }
};

class spell_pal_guardian_ancient_kings : public SpellScriptLoader
{
public:
    spell_pal_guardian_ancient_kings() : SpellScriptLoader("spell_pal_guardian_ancient_kings") { }

    class spell_pal_guardian_ancient_kings_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pal_guardian_ancient_kings_SpellScript)

        bool Load()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return false;
            return true;
        }

        bool Validate (SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_HOLY_GUARDIAN) ||
                !sSpellMgr->GetSpellInfo(SPELL_PALADIN_RETRI_GUARDIAN) ||
                !sSpellMgr->GetSpellInfo(SPELL_PALADIN_PROT_GUARDIAN))
                return false;
            return true;
        }

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetCaster())
            {
                if (caster->ToPlayer()->HasSpell(20473)) // Holy Shock
                {
                    caster->CastSpell(caster, SPELL_PALADIN_HOLY_GUARDIAN, false);
                    return;
                }
                if (caster->ToPlayer()->HasSpell(85256)) // Templar's Verdict
                {
                    caster->CastSpell(caster, SPELL_PALADIN_RETRI_GUARDIAN, false);
                    return;
                }
                if (caster->ToPlayer()->HasSpell(31935)) // Avenger's shield
                {
                    caster->CastSpell(caster, SPELL_PALADIN_PROT_GUARDIAN, false);
                    return;
                }
            }
        }

        void Register()
        {
            OnEffectLaunch += SpellEffectFn(spell_pal_guardian_ancient_kings_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pal_guardian_ancient_kings_SpellScript();
    }
};

class spell_pal_consecration : public SpellScriptLoader
{
public:
    spell_pal_consecration() : SpellScriptLoader("spell_pal_consecration") { }

    class spell_pal_consecration_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pal_consecration_AuraScript)
        
        float x, y, z;

        bool Load()
        {
           if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
               return false;

            return true;
         }

        bool Validate (SpellInfo const* /*spellEntry*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_CONSECRATION_DAMAGE) ||
                !sSpellMgr->GetSpellInfo(SPELL_PALADIN_CONSECRATION_SUMMON))
                return false;

            return true;
        }

        void HandlePeriodicDummy(AuraEffect const* aurEff)
        {
            uint64 consecrationNpcGUID = GetCaster()->m_SummonSlot[1];

            if (!consecrationNpcGUID)
               return;

            Unit* consecrationNpc = ObjectAccessor::GetCreature(*GetCaster(),consecrationNpcGUID);

            if (!consecrationNpc)
                return;

            consecrationNpc->GetPosition(x,y,z);
            consecrationNpc->CastSpell(x,y,z,SPELL_PALADIN_CONSECRATION_DAMAGE,true,NULL,NULL,GetCaster()->GetGUID());  
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_pal_consecration_AuraScript::HandlePeriodicDummy,EFFECT_1,SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pal_consecration_AuraScript();
    }
};

// Spell Id: 53600
class spell_pal_shield_of_righteous: public SpellScriptLoader
{
public:
    spell_pal_shield_of_righteous () :
            SpellScriptLoader("spell_pal_shield_of_righteous") { }

    class spell_pal_shield_of_righteous_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_pal_shield_of_righteous_SpellScript)

        void CalculateDamage (SpellEffIndex /*effIndex*/)
       {
            if (Unit* caster = GetCaster())
            {
                int32 damage = GetHitDamage();
                if (!damage)
                    damage = 13 * caster->getLevel();
                  
                damage += (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.05) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.12);
                switch (caster->GetPower(POWER_HOLY_POWER))
                {
                case 0:
                    damage = int32(damage * 1.16f);
                    break;
                case 1:
                    damage = int32((damage * 1.16f) * 3);
                    break;
                case 2:
                    damage = int32((damage * 1.16f) * 6);
                    break;
                }
                SetHitDamage(damage);
            }
        }

        void Register ()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_shield_of_righteous_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_pal_shield_of_righteous_SpellScript();
    }
};

// Light of Dawn
class spell_pal_light_of_dawn: public SpellScriptLoader
{
public:
    spell_pal_light_of_dawn () :
            SpellScriptLoader("spell_pal_light_of_dawn")
    {
    }

    class spell_pal_light_of_dawn_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_pal_light_of_dawn_SpellScript)
        ;

        uint32 totalheal;

        bool Load ()
        {
            if (GetCaster()->GetTypeId() != TYPEID_PLAYER)
                return false;

            return true;
        }

        void ChangeHeal (SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            Unit* target = GetHitUnit();

            if (!target)
                return;

            if (target == caster)
                return;

            switch (caster->GetPower(POWER_HOLY_POWER))
            {
            case 0:          // 1 Holy Power
            {
                totalheal = GetHitHeal();
                break;
            }
            case 1:          // 2 Holy Power
            {
                totalheal = GetHitHeal() * 2;
                break;
            }
            case 2:          // 3 Holy Power
            {
                totalheal = GetHitHeal() * 3;
                break;
            }
            }
            SetHitHeal(totalheal);
            caster->SetPower(POWER_HOLY_POWER, 0);
        }

        void Register ()
        {
            OnEffectLaunch += SpellEffectFn(spell_pal_light_of_dawn_SpellScript::ChangeHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_pal_light_of_dawn_SpellScript();
    }
};

enum Judgements
{
	SPELL_JUDGEMENT_TRIGGER           = 23590, 
	SPELL_JUDGEMENT_LONG_ARM_OF_LAWL  = 87173,
	SPELL_JUDGEMENT_WISE              = 31930,
	SPELL_JUDGEMENT_BOLD              = 89906,
	SPELL_JUDGEMENT_PURE              = 53655,
	SPELL_JUDGEMENT_ENLIGHTENED       = 87188,
};

/// Updated 4.3.4
class spell_pal_judgements : public SpellScriptLoader
{
    public:
        spell_pal_judgements() : SpellScriptLoader("spell_pal_judgements") { }

        class spell_pal_judgements_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pal_judgements_SpellScript);

            SpellCastResult CheckCast()
            {
         	   Unit* caster = GetCaster();
          	  if (caster->GetTypeId() != TYPEID_PLAYER)
              	  return SPELL_FAILED_DONT_REPORT;

         	   if (Unit* target = GetExplTargetUnit())
           	   {
               	   if (!target->IsFriendlyTo(caster) || target->getAttackers().empty())
                  	    return SPELL_FAILED_BAD_TARGETS;
           	   }
          	   else
               	 return SPELL_FAILED_BAD_TARGETS;

             return SPELL_CAST_OK;
            }

            void HandleDummy(SpellEffIndex effIndex)
            {
                PreventHitDefaultEffect(effIndex);
                Unit* caster = GetCaster();
                if (Unit* target = GetHitUnit())
		        {
                	caster->CastSpell(target, SPELL_JUDGEMENT_TRIGGER, true);
                	int32 regen = 0;

         	        if (caster->HasAura(89901)) // Jugements of the bold
                 	{
                   	   regen = int32(caster->GetCreateMana() * 0.025);
                   	   caster->CastCustomSpell(caster, SPELL_JUDGEMENT_BOLD, &regen, 0, 0, true);
		  	        }
         	  	    if (caster->HasAura(31878)) // Judgements of the Wise
                	{
                        regen = int32(caster->GetCreateMana() * 0.030);
                   	    caster->CastCustomSpell(caster, SPELL_JUDGEMENT_WISE, &regen, 0, 0, true);
		  	        }

		 	        if (caster->HasAura(87168)) // Long arm of Lawl Rank 1
				    {
                  		  if (roll_chance_i(50.0f))
                      		  if (caster->GetDistance(target) > 15.0f || !caster->IsWithinDistInMap(target, 15.0f)) 
                    				caster->CastSpell(caster, SPELL_JUDGEMENT_LONG_ARM_OF_LAWL, true);
			        }
		   	        else if (caster->HasAura(87172)) // Long arm of Lawl Rank 2
					{
                       	 if (caster->GetDistance(target) > 15.0f || !caster->IsWithinDistInMap(target, 15.0f)) 
                    	     caster->CastSpell(caster, SPELL_JUDGEMENT_LONG_ARM_OF_LAWL, true);
				    }

		 		    if (caster->HasAura(53671)) // Judgements of the Pure
                    		caster->CastSpell(caster, SPELL_JUDGEMENT_PURE, true);
		  	        if (caster->HasAura(53673)) 
                    		caster->CastSpell(caster, SPELL_JUDGEMENT_PURE+1, true);
		  	        if (caster->HasAura(54151)) 
                    		caster->CastSpell(caster, SPELL_JUDGEMENT_PURE+2, true);

		  	        if (caster->HasAura(53556)) // Enlightened Judgements
                    		caster->CastSpell(caster, SPELL_JUDGEMENT_ENLIGHTENED, true);
		  	        if (caster->HasAura(53557)) 
                    		caster->CastSpell(caster, SPELL_JUDGEMENT_ENLIGHTENED+1, true);
		        }
            } 


            void Register()
            {
                OnEffectHitTarget += SpellEffectFn(spell_pal_judgements_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const
        {
            return new spell_pal_judgements_SpellScript();
        }
};


// Spell Id: 31884
class spell_pal_avenging_shield: public SpellScriptLoader
{
public:
    spell_pal_avenging_shield () :
            SpellScriptLoader("spell_pal_avenging_shield") { }

    class spell_pal_avenging_shield_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_pal_avenging_shield_SpellScript)

        void CalculateDamage (SpellEffIndex /*effIndex*/)
       {
            if (Unit* caster = GetCaster())
            {
                int32 damage = GetHitDamage();
                if (!damage)
                    damage = 29 * caster->getLevel() + 14;
                  
                damage += (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.12) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.15);

                SetHitDamage(damage);
            }
        }

        void Register ()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_avenging_shield_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_pal_avenging_shield_SpellScript();
    }
};

// Spell Id: 79971
class spell_pal_hammer_of_wrath: public SpellScriptLoader
{
public:
    spell_pal_hammer_of_wrath () :
            SpellScriptLoader("spell_pal_hammer_of_wrath") { }

    class spell_pal_hammer_of_wrath_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_pal_hammer_of_wrath_SpellScript)

        void CalculateDamage (SpellEffIndex /*effIndex*/)
       {
            if (Unit* caster = GetCaster())
            {
                int32 damage = GetHitDamage();
                if (!damage)
                    damage = 36 * caster->getLevel() + 17;
                  
                damage += (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.2) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.2);

                SetHitDamage(damage);
            }
        }

        void Register ()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_hammer_of_wrath_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_pal_hammer_of_wrath_SpellScript();
    }
};

// Spell Id: 879
class spell_pal_exorcism: public SpellScriptLoader
{
public:
    spell_pal_exorcism () :
            SpellScriptLoader("spell_pal_exorcism") { }

    class spell_pal_exorcism_SpellScript: public SpellScript
    {
        PrepareSpellScript(spell_pal_exorcism_SpellScript)

        void CalculateDamage (SpellEffIndex /*effIndex*/)
       {
            if (Unit* caster = GetCaster())
            {
              int32 damage = (27 * caster->getLevel()) + 21;

                damage += (caster->ToPlayer()->GetTotalAttackPowerValue(BASE_ATTACK) * 0.08) + (caster->ToPlayer()->GetBaseSpellPowerBonus() * 0.23);
		  
              if (Aura* ancientpower = caster->GetAura(86700))
			  {
		       		damage += (damage * 0.34f) * ancientpower->GetStackAmount();
			  }

                SetHitDamage(damage);
            }
        }

        void Register ()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pal_exorcism_SpellScript::CalculateDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
        }
    };

    SpellScript* GetSpellScript () const
    {
        return new spell_pal_exorcism_SpellScript();
    }
};

// 85495,85498,85499 - Speed of Light
/// Updated 4.3.4
class spell_pal_speed_of_light : public SpellScriptLoader
{
    public:
        spell_pal_speed_of_light() : SpellScriptLoader("spell_pal_speed_of_light") { }

        class spell_pal_speed_of_light_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pal_speed_of_light_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/)
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PALADIN_SPEED_OF_LIGHT_BUFF))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                if (Unit* caster = GetCaster())
                    caster->CastCustomSpell(SPELL_PALADIN_SPEED_OF_LIGHT_BUFF, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), caster);
            }

            void Register()
            {
                OnEffectProc += AuraEffectProcFn(spell_pal_speed_of_light_AuraScript::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const
        {
            return new spell_pal_speed_of_light_AuraScript();
        }
};


void AddSC_paladin_spell_scripts()
{
    //new spell_pal_ardent_defender();
    new spell_pal_blessing_of_faith();
    new spell_pal_blessing_of_sanctuary();
    new spell_pal_divine_sacrifice();
    new spell_pal_divine_storm();
    new spell_pal_divine_storm_dummy();
    new spell_pal_exorcism_and_holy_wrath_damage();
	new spell_pal_grand_crusader();
    new spell_pal_eye_for_an_eye();
    new spell_pal_guarded_by_the_light();
    new spell_pal_hand_of_sacrifice();
    new spell_pal_hand_of_salvation();
    new spell_pal_holy_shock();
    new spell_pal_judgement_of_command();
    new spell_pal_lay_on_hands();
    new spell_pal_righteous_defense();
    new spell_pal_sacred_shield();
    new spell_pal_templar_s_verdict();
	new spell_pal_shield_of_the_righteous();
    new spell_pal_seal_of_righteousness();
    new spell_pal_word_of_glory();
    new spell_pal_selfless_healer();
    new spell_pal_guardian_ancient_kings();
    new spell_pal_consecration();
    new spell_pal_shield_of_righteous();
    new spell_pal_light_of_dawn();
    new spell_pal_judgements();
    new spell_pal_avenging_shield();
    new spell_pal_hammer_of_wrath();
    new spell_pal_exorcism(); 
	new spell_pal_speed_of_light();
}
