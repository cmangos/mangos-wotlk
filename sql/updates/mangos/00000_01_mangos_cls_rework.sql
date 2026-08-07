ALTER TABLE creature_template ADD COLUMN DamageMultiplierOLD FLOAT NOT NULL DEFAULT 1;
ALTER TABLE creature_template ADD COLUMN DamageVarianceOLD FLOAT NOT NULL DEFAULT 1;
UPDATE creature_template SET DamageMultiplierOLD=DamageMultiplier, DamageVarianceOLD= DamageVariance;
ALTER TABLE creature_template_classlevelstats ADD COLUMN BaseDamageExp0OLD FLOAT NOT NULL DEFAULT 0;
ALTER TABLE creature_template_classlevelstats ADD COLUMN BaseDamageExp1OLD FLOAT NOT NULL DEFAULT 0;
ALTER TABLE creature_template_classlevelstats ADD COLUMN BaseDamageExp2OLD FLOAT NOT NULL DEFAULT 0;
UPDATE creature_template_classlevelstats SET BaseDamageExp0OLD=BaseDamageExp0, BaseDamageExp1OLD= BaseDamageExp1, BaseDamageExp2OLD= BaseDamageExp2;
ALTER TABLE creature_template MODIFY DamageVariance FLOAT NOT NULL DEFAULT '0.4';


