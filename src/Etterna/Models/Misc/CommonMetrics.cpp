#include "Etterna/Globals/global.h"
#include "CommonMetrics.h"
#include "Foreach.h"
#include "Etterna/Singletons/GameManager.h"
#include "Etterna/Singletons/GameState.h"
#include "Etterna/Singletons/LuaManager.h"
#include "RageUtil/Utils/RageUtil.h"

ThemeMetric<RString> CommonMetrics::OPERATOR_MENU_SCREEN("Common",
														 "OperatorMenuScreen");
ThemeMetric<RString> CommonMetrics::DEFAULT_MODIFIERS("Common",
													  "DefaultModifiers");
LocalizedString CommonMetrics::WINDOW_TITLE("Common", "WindowTitle");
ThemeMetric<float> CommonMetrics::TICK_EARLY_SECONDS("ScreenGameplay",
													 "TickEarlySeconds");
ThemeMetric<RString> CommonMetrics::DEFAULT_NOTESKIN_NAME(
  "Common",
  "DefaultNoteSkinName");
ThemeMetricDifficultiesToShow CommonMetrics::DIFFICULTIES_TO_SHOW(
  "Common",
  "DifficultiesToShow");
ThemeMetricStepsTypesToShow CommonMetrics::STEPS_TYPES_TO_SHOW(
  "Common",
  "StepsTypesToHide");
ThemeMetric<bool> CommonMetrics::AUTO_SET_STYLE("Common", "AutoSetStyle");
ThemeMetric<int> CommonMetrics::PERCENT_SCORE_DECIMAL_PLACES(
  "Common",
  "PercentScoreDecimalPlaces");
ThemeMetric<RString> CommonMetrics::IMAGES_TO_CACHE("Common", "ImageCache");

ThemeMetricDifficultiesToShow::ThemeMetricDifficultiesToShow(
  const RString& sGroup,
  const RString& sName)
  : ThemeMetric<RString>(sGroup, sName)
{
	// re-read because ThemeMetric::ThemeMetric calls ThemeMetric::Read, not the
	// derived one
	if (IsLoaded())
		Read();
}
void
ThemeMetricDifficultiesToShow::Read()
{
	ASSERT(GetName().Right(6) == "ToShow");

	ThemeMetric<RString>::Read();

	m_v.clear();

	vector<RString> v;
	split(ThemeMetric<RString>::GetValue(), ",", v);
	if (v.empty()) {
		LuaHelpers::ReportScriptError(
		  "DifficultiesToShow must have at least one entry.");
		return;
	}

	FOREACH_CONST(RString, v, i)
	{
		Difficulty d = StringToDifficulty(*i);
		if (d == Difficulty_Invalid) {
			LuaHelpers::ReportScriptErrorFmt(
			  "Unknown difficulty \"%s\" in CourseDifficultiesToShow.",
			  i->c_str());
		} else {
			m_v.push_back(d);
		}
	}
}
const vector<Difficulty>&
ThemeMetricDifficultiesToShow::GetValue() const
{
	return m_v;
}

static void
RemoveStepsTypes(vector<StepsType>& inout, RString sStepsTypesToRemove)
{
	vector<RString> v;
	split(sStepsTypesToRemove, ",", v);
	if (v.size() == 0)
		return; // Nothing to do!

	// subtract StepsTypes
	FOREACH_CONST(RString, v, i)
	{
		StepsType st = GAMEMAN->StringToStepsType(*i);
		if (st == StepsType_Invalid) {
			LuaHelpers::ReportScriptErrorFmt(
			  "Invalid StepsType value '%s' in '%s'",
			  i->c_str(),
			  sStepsTypesToRemove.c_str());
			continue;
		}

		const vector<StepsType>::iterator iter =
		  find(inout.begin(), inout.end(), st);
		if (iter != inout.end())
			inout.erase(iter);
	}
}
ThemeMetricStepsTypesToShow::ThemeMetricStepsTypesToShow(const RString& sGroup,
														 const RString& sName)
  : ThemeMetric<RString>(sGroup, sName)
{
	// re-read because ThemeMetric::ThemeMetric calls ThemeMetric::Read, not the
	// derived one
	if (IsLoaded())
		Read();
}
void
ThemeMetricStepsTypesToShow::Read()
{
	ASSERT(GetName().Right(6) == "ToHide");

	ThemeMetric<RString>::Read();

	m_v.clear();
	GAMEMAN->GetStepsTypesForGame(GAMESTATE->m_pCurGame, m_v);

	RemoveStepsTypes(m_v, ThemeMetric<RString>::GetValue());
}
const vector<StepsType>&
ThemeMetricStepsTypesToShow::GetValue() const
{
	return m_v;
}

RString
CommonMetrics::LocalizeOptionItem(const RString& s, bool bOptional)
{
	if (bOptional && !THEME->HasString("OptionNames", s))
		return s;
	return THEME->GetString("OptionNames", s);
}

LuaFunction(LocalizeOptionItem,
			CommonMetrics::LocalizeOptionItem(SArg(1), true));
